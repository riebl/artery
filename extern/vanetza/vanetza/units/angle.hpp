#ifndef ANGLE_HPP_QWPLNJ3B
#define ANGLE_HPP_QWPLNJ3B

#include <boost/units/absolute.hpp>
#include <boost/units/quantity.hpp>
#include <boost/units/systems/angle/degrees.hpp>
#include <boost/units/systems/si/plane_angle.hpp>

namespace vanetza
{
namespace units
{

namespace si = boost::units::si;
using boost::units::degree::degree;
using boost::units::degree::degrees;
using boost::units::absolute;

typedef boost::units::quantity<boost::units::si::plane_angle> Angle;
typedef boost::units::quantity<boost::units::degree::plane_angle> GeoAngle;
typedef boost::units::quantity<boost::units::absolute<boost::units::degree::plane_angle>> TrueNorth;

BOOST_UNITS_STATIC_CONSTANT(true_north_degrees, TrueNorth::unit_type);

} // namespace units
} // namespace vanetza

#endif /* ANGLE_HPP_QWPLNJ3B */

