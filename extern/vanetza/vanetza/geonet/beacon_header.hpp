#ifndef BEACON_HEADER_HPP_1NRWPHXO
#define BEACON_HEADER_HPP_1NRWPHXO

#include <vanetza/geonet/position_vector.hpp>
#include <vanetza/geonet/serialization.hpp>

namespace vanetza
{
namespace geonet
{

struct BeaconHeader
{
public:
    static constexpr std::size_t length_bytes = LongPositionVector::length_bytes;

    LongPositionVector source_position;
};

void serialize(const BeaconHeader&, OutputArchive&);
void deserialize(BeaconHeader&, InputArchive&);

} // namespace geonet
} // namespace vanetza

#endif /* BEACON_HEADER_HPP_1NRWPHXO */

