#ifndef GEOMETRY_H_W8CYK9GM
#define GEOMETRY_H_W8CYK9GM

#include <boost/geometry/core/access.hpp>
#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/units/quantity.hpp>
#include <boost/units/systems/angle/degrees.hpp>
#include <boost/units/systems/si/length.hpp>
#include <boost/units/systems/si/plane_angle.hpp>
#include <iostream>

/**
 * Position represents a point in OMNeT++'s coordinate system
 *
 * Cartesian coordinate system of OMNeT++:
 * - origin is in the top left corner
 * - x axis is growing to the right
 * - y axis is growing to the bottom
 *
 * In accordance with INET, scalar values are in meters.
 */
struct Position
{
    using value_type = boost::units::quantity<boost::units::si::length>;

    Position() = default;
    Position(const Position&) = default;
    Position& operator=(const Position&) = default;

    explicit Position(double px, double py) :
        x(px * boost::units::si::meter),
        y(py * boost::units::si::meter) {}

    Position (value_type px , value_type py):
        x(px), y(py) {}

    value_type x;
    value_type y;
};

bool operator==(const Position&, const Position&);
bool operator!=(const Position&, const Position&);
Position::value_type distance(const Position&, const Position&);


/**
 * GeoPosition represents a horizontal geodetic datum (latitude and longitude)
 *
 * If not otherwise noted the datum refers to the WGS84 ellipsoid
 */
struct GeoPosition
{
    using value_type = boost::units::quantity<boost::units::degree::plane_angle>;

    GeoPosition() = default;
    GeoPosition(const GeoPosition&) = default;
    GeoPosition& operator=(const GeoPosition&) = default;

    value_type latitude;
    value_type longitude;
};


/**
 * OMNeT++ angle
 * - measured in radian
 * - 0 is headed east
 * - counter-clockwise orientation
 */
struct Angle
{
    using value_type = boost::units::quantity<boost::units::si::plane_angle>;

    Angle() = default;
    Angle(const Angle&) = default;
    Angle& operator=(const Angle&) = default;

    Angle(value_type angle) : value(angle) {}
    explicit Angle(double rad) : value(rad * boost::units::si::radian) {}

    double radian() const;
    double degree() const;

    value_type value;
};


namespace boost { namespace geometry { namespace traits
{

BOOST_GEOMETRY_DETAIL_SPECIALIZE_POINT_TRAITS(Position, 2, double, cs::cartesian)

template<> struct access<Position, 0>
{
    static inline double get(const Position& pos) { return pos.x.value(); }
    static inline void set(Position& pos, double v) { pos.x = Position::value_type::from_value(v); }
};

template<> struct access<Position, 1>
{
    static inline double get(const Position& pos) { return pos.y.value(); }
    static inline void set(Position& pos, double v) { pos.y = Position::value_type::from_value(v); }
};

} // namespace traits
} // namespace geometry
} // namespace boost

#endif /* GEOMETRY_H_W8CYK9GM */

