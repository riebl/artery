#include "buffer_packet.hpp"

namespace vanetza
{

BufferPacket::BufferPacket()
{
}

void BufferPacket::swap(OsiLayer layer, ByteBuffer& replacement)
{
    ByteBuffer& stored = mBuffers[layer];
    stored.swap(replacement);
}

const ByteBuffer& BufferPacket::operator[](OsiLayer layer) const
{
    auto match = mBuffers.find(layer);
    if (match == mBuffers.end()) {
        static const ByteBuffer scEmptyBuffer;
        return scEmptyBuffer;
    } else {
        return match->second;
    }
}

std::size_t BufferPacket::size() const
{
    std::size_t packet_size = 0;
    for (const auto& it : *this) {
        packet_size += it.second.size();
    }
    return packet_size;
}

void BufferPacket::clear()
{
    for (auto& it : mBuffers) {
        it.second.clear();
    }
}

} // namespace vanetza

