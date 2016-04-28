#ifndef PACKET_VARIANT_HPP_LILZ0UWN
#define PACKET_VARIANT_HPP_LILZ0UWN

#include <vanetza/common/byte_view.hpp>
#include <vanetza/common/serialization.hpp>
#include <vanetza/net/chunk_packet.hpp>
#include <vanetza/net/cohesive_packet.hpp>
#include <boost/variant.hpp>

namespace vanetza
{

using PacketVariant = boost::variant<ChunkPacket, CohesivePacket>;

inline std::size_t
size(const CohesivePacket& packet, OsiLayer from, OsiLayer to)
{
    return packet.size(from, to);
}

inline std::size_t
size(const CohesivePacket& packet, OsiLayer layer)
{
    return packet.size(layer);
}

inline std::size_t
size(const ChunkPacket& packet, OsiLayer from, OsiLayer to)
{
    return packet.size(from, to);
}

inline std::size_t
size(const ChunkPacket& packet, OsiLayer layer)
{
    return packet[layer].size();
}

/**
 * Create a view of a packet's bytes assigned to a certain layer
 * \param packet
 * \param layer
 * \return a byte view, possibly empty
 */
byte_view_range create_byte_view(const ChunkPacket&, OsiLayer);
byte_view_range create_byte_view(const CohesivePacket&, OsiLayer);

/**
 * Serialize a whole packet from physical to application layer
 * \param archive destination archive
 * \param packet source packet
 */
void serialize(OutputArchive&, const ChunkPacket&);
void serialize(OutputArchive&, const CohesivePacket&);

} // namespace vanetza

namespace boost
{

// PacketVariant is only a type alias, so we need this quirk for proper name lookup
std::size_t size(const vanetza::PacketVariant&, vanetza::OsiLayer from, vanetza::OsiLayer to);
std::size_t size(const vanetza::PacketVariant&, vanetza::OsiLayer);
vanetza::byte_view_range create_byte_view(const vanetza::PacketVariant&, vanetza::OsiLayer);
void serialize(vanetza::OutputArchive&, const vanetza::PacketVariant&);

} // namespace boost

#endif /* PACKET_VARIANT_HPP_LILZ0UWN */

