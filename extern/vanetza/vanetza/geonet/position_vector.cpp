#include "areas.hpp"
#include "position_vector.hpp"
#include "serialization.hpp"

namespace vanetza
{
namespace geonet
{

const std::size_t LongPositionVector::length_bytes;
const std::size_t ShortPositionVector::length_bytes;

LongPositionVector::LongPositionVector() : position_accuracy_indicator(false)
{
}

GeodeticPosition LongPositionVector::position() const
{
    return GeodeticPosition {
        static_cast<units::GeoAngle>(latitude),
        static_cast<units::GeoAngle>(longitude)
    };
}

ShortPositionVector::ShortPositionVector(const LongPositionVector& lpv) :
    gn_addr(lpv.gn_addr), timestamp(lpv.timestamp),
    latitude(lpv.latitude), longitude(lpv.longitude)
{
}

bool operator==(const LongPositionVector& lhs, const LongPositionVector& rhs)
{
    return lhs.gn_addr == rhs.gn_addr
        && lhs.timestamp == rhs.timestamp
        && lhs.latitude == rhs.latitude
        && lhs.longitude == rhs.longitude
        && lhs.speed == rhs.speed
        && lhs.heading == rhs.heading
        && lhs.position_accuracy_indicator == rhs.position_accuracy_indicator;
}

bool operator!=(const LongPositionVector& lhs, const LongPositionVector& rhs)
{
    return !(lhs == rhs);
}

bool operator==(const ShortPositionVector& lhs, const ShortPositionVector& rhs)
{
    return lhs.gn_addr == rhs.gn_addr
        && lhs.timestamp == rhs.timestamp
        && lhs.latitude == rhs.latitude
        && lhs.longitude == rhs.longitude;
}

bool operator!=(const ShortPositionVector& lhs, const ShortPositionVector& rhs)
{
    return !(lhs == rhs);
}

void serialize(const LongPositionVector& lpv, OutputArchive& ar)
{
    serialize(lpv.gn_addr, ar);
    serialize(lpv.timestamp, ar);
    serialize(lpv.latitude, ar);
    serialize(lpv.longitude, ar);
    uint16_t paiAndSpeed = lpv.speed.value().raw();
    paiAndSpeed |= lpv.position_accuracy_indicator ?  0x8000 : 0x0000;
    serialize(host_cast(paiAndSpeed), ar);
    serialize(lpv.heading, ar);
}

void deserialize(LongPositionVector& lpv, InputArchive& ar)
{
    deserialize(lpv.gn_addr, ar);
    deserialize(lpv.timestamp, ar);
    deserialize(lpv.latitude, ar);
    deserialize(lpv.longitude, ar);
    uint16_t paiAndSpeed = 0;
    deserialize(paiAndSpeed, ar);
    lpv.position_accuracy_indicator = ((paiAndSpeed & 0x8000) != 0);
    lpv.speed = LongPositionVector::speed_u15t::from_value(paiAndSpeed);
    deserialize(lpv.heading, ar);
}

} // namespace geonet
} // namespace vanetza
