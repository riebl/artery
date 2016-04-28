#include "packet_buffer.hpp"
#include "basic_header.hpp"

namespace vanetza
{
namespace geonet
{
namespace packet_buffer
{

Expiry::Expiry(Timestamp now, Lifetime lifetime) :
    m_buffered_since(now), m_expires_at(now)
{
    m_expires_at += Timestamp::duration_type(lifetime.decode());
}

bool Expiry::is_expired(Timestamp now) const
{
    return (m_expires_at < now);
}

} // namespace packet_buffer


PacketBuffer::PacketBuffer(std::size_t capacity) :
    m_capacity(capacity), m_stored(0)
{
}

bool PacketBuffer::push(data_ptr data, Timestamp now)
{
    assert(data);
    bool pushed = false;
    drop_expired(now);
    if (drop(data->length())) {
        m_stored += data->length();
        m_nodes.push_back(std::make_tuple(
                    expiry_type(now, data->lifetime()),
                    std::move(data)
                ));
        pushed = true;
    }
    return pushed;
}

std::list<NextHop> PacketBuffer::flush(Timestamp now)
{
    decltype(m_nodes) nodes;
    std::swap(m_nodes, nodes);
    m_stored = 0;

    std::list<NextHop> flushed;
    for (auto& node : nodes) {
        const auto& expiry = std::get<0>(node);
        if (!expiry.is_expired(now)) {
            auto& data = std::get<1>(node);
            const auto queuing_time = now - expiry.buffered_since();
            units::Duration lifetime = data->lifetime().decode();
            lifetime -= units::Duration(queuing_time);
            data->lifetime().encode(lifetime);

            NextHop next_hop = data->flush();
            if (next_hop.valid()) {
                flushed.push_back(std::move(next_hop));
            }
        }
    }
    return flushed;
}

bool PacketBuffer::drop_head()
{
    bool dropped = !m_nodes.empty();
    if (dropped) {
        m_stored -= std::get<1>(m_nodes.front())->length();
        m_nodes.pop_front();
    }
    return dropped;
}

void PacketBuffer::drop_expired(Timestamp now)
{
    std::list<decltype(m_nodes)::iterator> expired;
    for (auto it = m_nodes.begin(), end = m_nodes.end(); it != end;) {
        if (std::get<0>(*it).is_expired(now)) {
            m_stored -= std::get<1>(*it)->length();
            it = m_nodes.erase(it);
        } else {
            ++it;
        }
    }
}

bool PacketBuffer::drop(std::size_t bytes)
{
    while (free() < bytes && drop_head()) {}
    return free() >= bytes;
}

} // namespace geonet
} // namespace vanetza

