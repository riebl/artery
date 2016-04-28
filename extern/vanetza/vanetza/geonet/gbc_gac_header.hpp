#ifndef GBC_GAC_HEADER_HPP_FS5DH20M
#define GBC_GAC_HEADER_HPP_FS5DH20M

#include <vanetza/geonet/areas.hpp>
#include <vanetza/geonet/position_vector.hpp>
#include <vanetza/geonet/sequence_number.hpp>
#include <vanetza/geonet/serialization.hpp>
#include <vanetza/geonet/units.hpp>
#include <cstddef>

namespace vanetza
{
namespace geonet
{
namespace detail
{

/**
 * GeoBroadcast GUC and GeoAnycast GAC have identical header layout.
 * We use this class as common base class to reduce duplicate code.
 */
struct GbcGacHeader
{
    static const std::size_t length_bytes = 20 + LongPositionVector::length_bytes;

    SequenceNumber sequence_number;
    uint16_t reserved1;
    LongPositionVector source_position;
    geo_angle_i32t geo_area_pos_latitude;
    geo_angle_i32t geo_area_pos_longitude;
    distance_u16t distance_a;
    distance_u16t distance_b;
    angle_u16t angle;
    uint16_t reserved2;

    void destination(const Area&);
    GeodeticPosition position() const;
    void position(const GeodeticPosition&);

protected:
    Area destination(const decltype(Area::shape)&) const;
};

void serialize(const GbcGacHeader&, OutputArchive&);
void deserialize(GbcGacHeader&, InputArchive&);

} // namespace detail
} // namespace geonet
} // namespace vanetza

#endif /* GBC_GAC_HEADER_HPP_FS5DH20M */

