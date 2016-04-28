#include <vanetza/facilities/path_point.hpp>
#include <boost/units/cmath.hpp>
#include <boost/units/systems/si/prefixes.hpp>

namespace vanetza
{
namespace facilities
{

const units::Length cREarthMeridian = units::Length(6378.137 * units::si::kilo * units::si::meters);

PathPoint::PathPoint()
{
}

units::Length chord_length(const PathPoint& a, const PathPoint& b)
{
    const units::Angle lat1(a.latitude);
    const units::Angle lon1(a.longitude);
    const units::Angle lat2(b.latitude);
    const units::Angle lon2(b.longitude);

    return cREarthMeridian *
        acos(cos(lat1) * cos(lat2) * cos(lon1 - lon2) + sin(lat1) * sin(lat2))
        / units::si::radian ;
}

} // namespace facilities
} // namespace vanetza
