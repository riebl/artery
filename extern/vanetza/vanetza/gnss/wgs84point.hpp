#ifndef WGS84POINT_HPP_GAHDJKCG
#define WGS84POINT_HPP_GAHDJKCG

#include <vanetza/units/angle.hpp>

namespace vanetza
{

struct Wgs84Point
{
    typedef units::GeoAngle angle_type;
    Wgs84Point(angle_type latitude, angle_type longitude) : lat(latitude), lon(longitude) {}
    angle_type lat;
    angle_type lon;
};

} // namespace vanetza 

#endif /* WGS84POINT_HPP_GAHDJKCG */

