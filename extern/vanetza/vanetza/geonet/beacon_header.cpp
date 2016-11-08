#include "beacon_header.hpp"
#include "serialization.hpp"

namespace vanetza
{
namespace geonet
{

constexpr std::size_t BeaconHeader::length_bytes;

void serialize(const BeaconHeader& hdr, OutputArchive& ar)
{
    serialize(hdr.source_position, ar);
}

void deserialize(BeaconHeader& hdr, InputArchive& ar)
{
    deserialize(hdr.source_position, ar);
}

} // namespace geonet
} // namespace vanetza

