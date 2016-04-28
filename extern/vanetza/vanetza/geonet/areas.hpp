#ifndef AREAS_HPP_CVK1NIAI
#define AREAS_HPP_CVK1NIAI

#include <vanetza/units/angle.hpp>
#include <vanetza/units/area.hpp>
#include <vanetza/units/length.hpp>
#include <boost/variant.hpp>

namespace vanetza
{
namespace geonet
{

/**
 * Cartesian position.
 * As it is not strictly specified in any GeoNet standard document,
 * I define it as point in an ENU coordinate system and its units as meters.
 */
struct CartesianPosition
{
    CartesianPosition() : x(0.0 * units::si::meter), y(0.0 * units::si::meter) {}
    CartesianPosition(units::Length x_, units::Length y_) : x(x_), y(y_) {}
    units::Length x;
    units::Length y;
};

CartesianPosition operator-(const CartesianPosition&, const CartesianPosition&);

struct GeodeticPosition
{
    GeodeticPosition() :
        latitude(0.0 * units::degree), longitude(0.0 * units::degree) {}
    GeodeticPosition(units::GeoAngle lat, units::GeoAngle lon) :
        latitude(lat), longitude(lon) {}
    units::GeoAngle latitude;
    units::GeoAngle longitude;
};

/**
 * Get distance between two geodetic positions on WGS84 ellipsoid
 * \param lhs left hand side
 * \param rhs right hand side
 * \return distance in meters, always positive
 */
units::Length distance(const GeodeticPosition& lhs, const GeodeticPosition& rhs);

struct Circle
{
    Circle() : r(1.0 * units::si::meters) {}
    units::Length r; // radius
};

struct Rectangle
{
    Rectangle() : a(1.0 * units::si::meters), b(1.0 * units::si::meters) {}
    units::Length a; // center to long side
    units::Length b; // center to short side
};

struct Ellipse
{
    Ellipse() : a(1.0 * units::si::meters), b(1.0 * units::si::meters) {}
    units::Length a; // long semi-axis
    units::Length b; // short semi-axis
};

struct Area
{
    boost::variant<Rectangle, Ellipse, Circle> shape;
    GeodeticPosition position;
    units::Angle angle;
};

double geometric_function(const Circle&, const CartesianPosition&);
double geometric_function(const Rectangle&, const CartesianPosition&);
double geometric_function(const Ellipse&, const CartesianPosition&);
double geometric_function(const decltype(Area::shape)&, const CartesianPosition&);

/**
 * Derive cartesian position ENU from geodetic WGS84 coordinates
 * and a WGS84 reference point which becomes the cartesian origin.
 * \param origin WGS84 reference point becoming origin
 * \param position Calculate cartesian coordinates for this point
 * \return Cartesian coordinates of position relative to origin
 */
CartesianPosition local_cartesian(
        const GeodeticPosition& origin,
        const GeodeticPosition& position);

/**
 * Canonicalize a point relative to the center point of a shape and
 * the azimuth angle of this shape, i.e. angle between north and its
 * long side.
 * \param point Point to canonicalize
 * \param shape Center of shape
 * \param azimuth Azimuth angle
 * \return canonical position
 */
CartesianPosition canonicalize(const CartesianPosition& point, const CartesianPosition& shape, units::Angle azimuth);

/**
 * Check if positon is within or at border of area
 * \param area with shape, dimensions, azimuth and center point position
 * \param position Geodetic position to check against area
 * \return true if position is inside or at border
 */
bool inside_or_at_border(const Area&, const GeodeticPosition&);

/**
 * Calculate area size in square km.
 * \param area Area object
 * \return area size
 */
units::Area area_size(const Area&);

template<class SHAPE>
bool inside_shape(const SHAPE& shape, const CartesianPosition& p)
{
    return geometric_function(shape, p) > 0.0;
}

template<class SHAPE>
bool outside_shape(const SHAPE& shape, const CartesianPosition& p)
{
    return geometric_function(shape, p) < 0.0;
}

template<class SHAPE>
bool at_shape_border(const SHAPE& shape, const CartesianPosition& p)
{
    return geometric_function(shape, p) == 0.0;
}

template<class SHAPE>
bool at_center_point(const SHAPE& shape, const CartesianPosition& p)
{
    return geometric_function(shape, p) == 1.0;
}

} // namespace geonet
} // namespace vanetza

#endif /* AREAS_HPP_CVK1NIAI */

