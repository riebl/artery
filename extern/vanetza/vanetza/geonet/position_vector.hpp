#ifndef POSITION_VECTOR_HPP_WJAGEOCS
#define POSITION_VECTOR_HPP_WJAGEOCS

#include <vanetza/common/bit_number.hpp>
#include <vanetza/geonet/address.hpp>
#include <vanetza/geonet/serialization.hpp>
#include <vanetza/geonet/timestamp.hpp>
#include <vanetza/geonet/units.hpp>
#include <boost/units/quantity.hpp>
#include <boost/units/systems/si/velocity.hpp>
#include <cstdint>

namespace vanetza
{
namespace geonet
{

struct GeodeticPosition;

class LongPositionVector
{
public:
    static const std::size_t length_bytes = 24;
    typedef boost::units::quantity<boost::units::make_scaled_unit<
                boost::units::si::velocity,
                boost::units::scale<10, boost::units::static_rational<-2>>
            >::type, BitNumber<uint_fast16_t, 15>> speed_u15t; // 1/100 m/s

    Address gn_addr;
    Timestamp timestamp; // ms since 2004-01-01 00:00:00.000
    geo_angle_i32t latitude;
    geo_angle_i32t longitude;
    bool position_accuracy_indicator;
    speed_u15t speed;
    heading_u16t heading;

    LongPositionVector();
    GeodeticPosition position() const;
};

bool operator==(const LongPositionVector& lhs, const LongPositionVector& rhs);
bool operator!=(const LongPositionVector& lhs, const LongPositionVector& rhs);

void serialize(const LongPositionVector&, OutputArchive&);
void deserialize(LongPositionVector&, InputArchive&);

class ShortPositionVector
{
public:
    static const std::size_t length_bytes = 20;

    ShortPositionVector() = default;
    ShortPositionVector(const ShortPositionVector&) = default;
    ShortPositionVector& operator=(const ShortPositionVector&) = default;
    explicit ShortPositionVector(const LongPositionVector&);

    Address gn_addr;
    Timestamp timestamp;
    geo_angle_i32t latitude;
    geo_angle_i32t longitude;
};

bool operator==(const ShortPositionVector& lhs, const ShortPositionVector& rhs);
bool operator!=(const ShortPositionVector& lhs, const ShortPositionVector& rhs);

} // namespace geonet
} // namespace vanetza

#endif /* POSITION_VECTOR_HPP_WJAGEOCS */

