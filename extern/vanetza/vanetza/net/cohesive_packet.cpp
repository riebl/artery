#include "cohesive_packet.hpp"
#include <cassert>
#include <iterator>

namespace vanetza
{

constexpr unsigned layer_index(OsiLayer layer)
{
    return static_cast<unsigned>(layer);
}

static_assert(layer_index(min_osi_layer()) == 1, "Lowest OSI layer index broken");


CohesivePacket::CohesivePacket(const ByteBuffer& buffer, OsiLayer layer) :
    m_buffer(buffer)
{
    reset_iterators(layer);
}

CohesivePacket::CohesivePacket(ByteBuffer&& buffer, OsiLayer layer) :
    m_buffer(std::move(buffer))
{
    reset_iterators(layer);
}

CohesivePacket::CohesivePacket(const CohesivePacket& other) :
    m_buffer(other.m_buffer)
{
    rebuild_iterators(other);
}

CohesivePacket& CohesivePacket::operator=(const CohesivePacket& other)
{
    m_buffer = other.m_buffer;
    rebuild_iterators(other);
    return *this;
}

auto CohesivePacket::operator[](OsiLayer layer) const -> buffer_const_range
{
    return get(layer_index(layer));
}

auto CohesivePacket::operator[](OsiLayer layer) -> buffer_range
{
    return get(layer_index(layer));
}

void CohesivePacket::set_boundary(OsiLayer layer, unsigned bytes)
{
    const unsigned layer_idx = layer_index(layer);
    assert(get(layer_idx).size() >= bytes);
    m_iterators[layer_idx] = m_iterators[layer_idx - 1] + bytes;
}

void CohesivePacket::trim(OsiLayer from, unsigned bytes)
{
    if (size(from, max_osi_layer()) > bytes) {
        const auto from_idx = layer_index(from) - 1;
        const auto max_idx = layer_index(max_osi_layer());
        m_iterators[max_idx] = m_iterators[from_idx] + bytes;
        assert(&m_iterators.back() == &m_iterators[max_idx]);
        assert(m_iterators.back() >= m_iterators.front());
        for (auto idx = from_idx; idx < max_idx; ++idx) {
            if (m_iterators[idx] > m_iterators[max_idx]) {
                m_iterators[idx] = m_iterators[max_idx];
            }
        }
    }
    assert(size(from, max_osi_layer()) <= bytes);
}

std::size_t CohesivePacket::size() const
{
    return std::distance(m_iterators.front(), m_iterators.back());
}

std::size_t CohesivePacket::size(OsiLayer single_layer) const
{
    return get(layer_index(single_layer)).size();
}

std::size_t CohesivePacket::size(OsiLayer from, OsiLayer to) const
{
    auto begin = m_iterators[layer_index(from) - 1];
    auto end = m_iterators[layer_index(to)];
    auto dist = std::distance(begin, end);
    return dist < 0 ? 0 : dist;
}

void CohesivePacket::reset_iterators(OsiLayer ins_layer)
{
    unsigned layer_idx = 0;

    const unsigned ins_layer_idx = layer_index(ins_layer);
    for (unsigned i = 0; i < ins_layer_idx; ++i) {
        m_iterators[layer_idx++] = m_buffer.begin();
    }

    const unsigned max_layer_idx = layer_index(max_osi_layer());
    for (unsigned i = ins_layer_idx; i <= max_layer_idx; ++i) {
        m_iterators[layer_idx++] = m_buffer.end();
    }

    assert(m_iterators.size() == layer_idx);
}

void CohesivePacket::rebuild_iterators(const CohesivePacket& other)
{
    assert(m_buffer.size() == other.m_buffer.size());
    m_iterators.front() = m_buffer.begin();
    auto next = m_iterators.front();
    for (unsigned i = 1; i < m_iterators.size(); ++i) {
        next += other.m_iterators[i] - other.m_iterators[i - 1];
        m_iterators[i] = next;
    }
}

auto CohesivePacket::get(unsigned layer_idx) -> buffer_range
{
    assert(layer_idx > 0);
    assert(layer_idx < m_iterators.size());
    return buffer_range(m_iterators[layer_idx - 1], m_iterators[layer_idx]);
}

auto CohesivePacket::get(unsigned layer_idx) const -> buffer_const_range
{
    assert(layer_idx > 0);
    assert(layer_idx < m_iterators.size());
    return buffer_const_range(m_iterators[layer_idx - 1], m_iterators[layer_idx]);
}

} // namespace vanetza

