#ifndef PACKETS_HPP_XB5AOPWE
#define PACKETS_HPP_XB5AOPWE

#include <vanetza/net/packet_variant.hpp>

namespace vanetza
{

using DownPacket = ChunkPacket;
using UpPacket = PacketVariant;

/**
 * Create clone of a packet
 */
std::unique_ptr<DownPacket> duplicate(const DownPacket&);
std::unique_ptr<DownPacket> duplicate(const UpPacket&);

} // namespace vanetza

#endif /* PACKETS_HPP_XB5AOPWE */

