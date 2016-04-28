#ifndef PATH_POINT_HPP_LXQ9YZKI
#define PATH_POINT_HPP_LXQ9YZKI

#include <vanetza/units/angle.hpp>
#include <vanetza/units/length.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace vanetza
{
namespace facilities
{

extern const units::Length cREarthMeridian;

struct PathPoint
{
    PathPoint();

    units::GeoAngle latitude;
    units::GeoAngle longitude;
    units::Angle heading;
    boost::posix_time::ptime time;
};

/**
 * Calculate chord length between two points
 * \param path point A
 * \param path point B
 * \return chord length
 */
units::Length chord_length(const PathPoint&, const PathPoint&);

} // namespace facilities
} // namespace vanetza

#endif /* PATH_POINT_HPP_LXQ9YZKI */

