#include "chunk_packet.hpp"
#include "cohesive_packet.hpp"
#include <cassert>

namespace vanetza
{

static const ByteBufferConvertible empty_byte_buffer_convertible;

ChunkPacket::ChunkPacket()
{
}

ChunkPacket::ChunkPacket(const ChunkPacket& other)
{
    for (auto& layer : other.m_layers) {
        m_layers.insert(layer);
    }
}

ChunkPacket& ChunkPacket::operator=(const ChunkPacket& other)
{
    ChunkPacket tmp = other;
    m_layers.swap(tmp.m_layers);
    return *this;
}

ByteBufferConvertible& ChunkPacket::layer(OsiLayer layer)
{
    return m_layers[layer];
}

const ByteBufferConvertible& ChunkPacket::layer(OsiLayer layer) const
{
    auto found = m_layers.find(layer);
    if (found != m_layers.end()) {
        assert(found->first == layer);
        return found->second;
    } else {
        return empty_byte_buffer_convertible;
    }
}

std::size_t ChunkPacket::size() const
{
    std::size_t size = 0;
    for(auto& it : m_layers)
    {
        size += it.second.size();
    }

    return size;
}

std::size_t ChunkPacket::size(OsiLayer from, OsiLayer to) const
{
    assert(from <= to);
    std::size_t size = 0;
    for (auto& layer : m_layers) {
        if (layer.first >= from && layer.first <= to) {
            size += layer.second.size();
        }
    }
    return size;
}

ChunkPacket ChunkPacket::extract(OsiLayer from, OsiLayer to)
{
    ChunkPacket result;
    for (auto layer : osi_layer_range(from, to)) {
        using namespace std;
        swap(result[layer], (*this)[layer]);
    }
    return result;
}

ChunkPacket& ChunkPacket::merge(ChunkPacket& source, OsiLayer from, OsiLayer to)
{
    for (auto layer : osi_layer_range(from, to)) {
        (*this)[layer] = std::move(source[layer]);
        source[layer] = empty_byte_buffer_convertible;
    }
    return *this;
}

} // namespace vanetza

