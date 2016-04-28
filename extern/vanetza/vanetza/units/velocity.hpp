#ifndef VELOCITY_HPP_0BLFAIDT
#define VELOCITY_HPP_0BLFAIDT

#include <boost/units/quantity.hpp>
#include <boost/units/base_units/metric/knot.hpp>
#include <boost/units/systems/si/velocity.hpp>

namespace vanetza
{
namespace units
{

namespace metric
{

BOOST_UNITS_STATIC_CONSTANT(knot, boost::units::metric::knot_base_unit::unit_type);
BOOST_UNITS_STATIC_CONSTANT(knots, boost::units::metric::knot_base_unit::unit_type);

} // namespace metric

typedef boost::units::quantity<boost::units::si::velocity> Velocity;
typedef boost::units::quantity<boost::units::metric::knot_base_unit::unit_type> NauticalVelocity;

} // namespace units
} // namespace vanetza

#endif /* VELOCITY_HPP_0BLFAIDT */

