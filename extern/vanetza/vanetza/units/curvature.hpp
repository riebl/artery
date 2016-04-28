#ifndef RECIPROCALLENGTH_HPP_6VHZDL7G
#define RECIPROCALLENGTH_HPP_6VHZDL7G

#include <boost/units/derived_dimension.hpp>
#include <boost/units/physical_dimensions/length.hpp>
#include <boost/units/systems/si/base.hpp>
#include <boost/units/quantity.hpp>

namespace vanetza
{
namespace units
{

typedef boost::units::derived_dimension<boost::units::length_base_dimension, -1>::type curvature_dimension;
typedef boost::units::unit<curvature_dimension, boost::units::si::system> curvature;
typedef boost::units::quantity<curvature> Curvature;

BOOST_UNITS_STATIC_CONSTANT(reciprocal_metre, curvature);

} // namespace units
} // namespace vanetza

#endif /* RECIPROCALLENGTH_HPP_6VHZDL7G */