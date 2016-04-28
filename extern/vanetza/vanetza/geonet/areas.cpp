#include "areas.hpp"
#include <boost/math/constants/constants.hpp>
#include <boost/units/cmath.hpp>
#include <GeographicLib/Geocentric.hpp>
#include <GeographicLib/Geodesic.hpp>
#include <GeographicLib/LocalCartesian.hpp>
#include <algorithm>
#include <cassert>

namespace vanetza
{
namespace geonet
{

CartesianPosition operator-(const CartesianPosition& a, const CartesianPosition& b)
{
    return CartesianPosition { a.x - b.x, a.y - b.y };
}

double geometric_function(const Circle& c, const CartesianPosition& p)
{
    assert(c.r.value() != 0.0);
    const double x_over_r = p.x / c.r;
    const double y_over_r = p.y / c.r;
    return 1.0 - (x_over_r * x_over_r) - (y_over_r * y_over_r);
}

double geometric_function(const Rectangle& r, const CartesianPosition& p)
{
    assert(r.a.value() != 0.0);
    assert(r.b.value() != 0.0);
    const double x_over_a = p.x / r.a;
    const double y_over_b = p.y / r.b;
    return std::min(1.0 - x_over_a * x_over_a, 1.0 - y_over_b * y_over_b);
}

double geometric_function(const Ellipse& e, const CartesianPosition& p)
{
    assert(e.a.value() != 0.0);
    assert(e.b.value() != 0.0);
    const double x_over_a = p.x / e.a;
    const double y_over_b = p.y / e.b;
    return 1.0 - (x_over_a * x_over_a) - (y_over_b * y_over_b);
}

struct geometric_function_visitor : public boost::static_visitor<double>
{
    geometric_function_visitor(const CartesianPosition& p) : point(p) {}

    template<class SHAPE>
    double operator()(const SHAPE& s) const
    {
        return geometric_function(s, point);
    }

    const CartesianPosition& point;
};

double geometric_function(const decltype(Area::shape)& shape, const CartesianPosition& p)
{
    geometric_function_visitor visitor(p);
    return boost::apply_visitor(visitor, shape);
}

CartesianPosition local_cartesian(
        const GeodeticPosition& origin,
        const GeodeticPosition& position)
{
    namespace si = units::si;
    using namespace GeographicLib;
    const Geocentric& earth = Geocentric::WGS84();
    LocalCartesian proj {
            origin.latitude / units::degree,
            origin.longitude / units::degree,
            0.0, earth
    };
    double result_x, result_y, unused_z = 0.0;
    proj.Forward(position.latitude / units::degree,
            position.longitude / units::degree, 0.0,
            result_x, result_y, unused_z);
    return CartesianPosition(result_x * si::meter, result_y * si::meter);
}

CartesianPosition canonicalize(
        const CartesianPosition& point,
        const CartesianPosition& shape, units::Angle azimuth)
{
    // move point (shape center is in origin afterwards)
    CartesianPosition moved = point - shape;
    // rotate resulting point around origin clockwise: zenith = 90 deg - azimuth
    // other interpretation: rotate shape's long side on abscissa
    const units::Angle zenith = units::Angle(90.0 * units::degree) - azimuth;
    CartesianPosition result;
    result.x = cos(zenith) * moved.x + sin(zenith) * moved.y;
    result.y = -sin(zenith) * moved.x + cos(zenith) * moved.y;
    return result;
}

struct area_size_visitor : public boost::static_visitor<units::Area>
{
    units::Area operator()(const Circle& circle) const
    {
        using namespace boost::math::double_constants;
        return pi * circle.r * circle.r;
    }

    units::Area operator()(const Rectangle& rectangle) const
    {
        using namespace boost::math::double_constants;
        return 4.0 * rectangle.a * rectangle.b;
    }

    units::Area operator()(const Ellipse& ellipse) const
    {
        using namespace boost::math::double_constants;
        return pi * ellipse.a * ellipse.b;
    }
};

units::Area area_size(const Area& area)
{
    return boost::apply_visitor(area_size_visitor(), area.shape);
}

units::Length distance(const GeodeticPosition& lhs, const GeodeticPosition& rhs)
{
    using namespace GeographicLib;
    const Geodesic& geod = Geodesic::WGS84();
    double distance_m = 0.0;
    geod.Inverse(lhs.latitude / units::degree, lhs.longitude / units::degree,
            rhs.latitude / units::degree, rhs.longitude / units::degree,
            distance_m);
    return distance_m * units::si::meters;
}

bool inside_or_at_border(const Area& area, const GeodeticPosition& geo_position)
{
    using units::si::meter;
    CartesianPosition canonical_position = canonicalize(
            CartesianPosition(0.0 * meter, 0.0 * meter),
            local_cartesian(area.position, geo_position),
            area.angle);
    return !outside_shape(area.shape, canonical_position);
}

} // namespace geonet
} // namespace vanetza

