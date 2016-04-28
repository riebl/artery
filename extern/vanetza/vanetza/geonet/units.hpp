#ifndef UNITS_HPP_O254IL9W
#define UNITS_HPP_O254IL9W

#include <boost/units/make_scaled_unit.hpp>
#include <boost/units/quantity.hpp>
#include <boost/units/systems/angle/degrees.hpp>
#include <boost/units/systems/si/length.hpp>
#include <cstdint>

namespace vanetza
{
namespace geonet
{

// distance given in meter
typedef boost::units::quantity<boost::units::si::length, uint16_t> distance_u16t;

// geographical angles with 1/10 micro degree resolution
typedef boost::units::quantity<boost::units::make_scaled_unit<
            boost::units::degree::plane_angle,
            boost::units::scale<10, boost::units::static_rational<-7>>
        >::type, int32_t> geo_angle_i32t;

// angle with 1 degree resolution
typedef boost::units::quantity<boost::units::degree::plane_angle, uint16_t> angle_u16t;

// heading with 0.1 degree resolution
typedef boost::units::quantity<boost::units::make_scaled_unit<
            boost::units::degree::plane_angle,
            boost::units::scale<10, boost::units::static_rational<-1>>
        >::type, uint16_t> heading_u16t;

} // namespace geonet
} // namespace vanetza

#endif /* UNITS_HPP_O254IL9W */

