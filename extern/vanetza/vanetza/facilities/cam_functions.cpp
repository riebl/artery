#ifndef CAM_FUNCTIONS_CPP_JQXFKSJP
#define CAM_FUNCTIONS_CPP_JQXFKSJP

#include <vanetza/asn1/asn1c_wrapper.hpp>
#include <vanetza/asn1/gen/asn_SEQUENCE_OF.h>
#include <vanetza/asn1/gen/BasicVehicleContainerLowFrequency.h>
#include <vanetza/asn1/gen/DeltaReferencePosition.h>
#include <vanetza/asn1/gen/PathDeltaTime.h>
#include <vanetza/facilities/cam_functions.hpp>
#include <vanetza/facilities/path_history.hpp>
#include <vanetza/geonet/areas.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/units/cmath.hpp>
#include <boost/units/systems/si/prefixes.hpp>
#include <boost/units/systems/angle/degrees.hpp>
#include <algorithm>
#include <limits>
#undef min

namespace vanetza
{
namespace facilities
{

using vanetza::units::Angle;

// TODO:  C2C-CC BSP allows up to 500m history for CAMs, we provide just minimal required history
void copy(const facilities::PathHistory& ph, BasicVehicleContainerLowFrequency& container)
{
    static const std::size_t scMaxPathPoints = 23;
    static const boost::posix_time::time_duration scMaxDeltaTime = boost::posix_time::millisec(655350);
    static const auto scMicrodegree = units::si::micro * units::degree;

    const auto& concise_points = ph.getConcisePoints();
    const facilities::PathPoint& ref = ph.getReferencePoint();
    std::size_t path_points = 0;

    for (const PathPoint& point : concise_points) {
        auto delta_time = ref.time - point.time; // positive: point is in past
        auto delta_latitude = point.latitude - ref.latitude; // positive: point is north
        auto delta_longitude = point.longitude - ref.longitude; // positive: point is east

        while (!delta_time.is_negative() && path_points < scMaxPathPoints) {
            ::PathPoint* path_point = asn1::allocate<::PathPoint>();
            path_point->pathDeltaTime = asn1::allocate<PathDeltaTime_t>();
            *(path_point->pathDeltaTime) = std::min(delta_time, scMaxDeltaTime).total_milliseconds() /
                10 * PathDeltaTime::PathDeltaTime_tenMilliSecondsInPast;
            path_point->pathPosition.deltaLatitude = (delta_latitude / scMicrodegree).value() *
                DeltaLatitude::DeltaLatitude_oneMicrodegreeNorth;
            path_point->pathPosition.deltaLongitude = (delta_longitude / scMicrodegree).value() *
                DeltaLongitude::DeltaLongitude_oneMicrodegreeEast;
            path_point->pathPosition.deltaAltitude = DeltaAltitude::DeltaAltitude_unavailable;

            ASN_SEQUENCE_ADD(&container.pathHistory, path_point);

            delta_time -= scMaxDeltaTime;
            ++path_points;
        }
    }
}

bool similar_heading(const Heading& a, const Heading& b, Angle limit)
{
    // HeadingValues are tenth of degree (900 equals 90 degree east)
    static_assert(HeadingValue_wgs84East == 900, "HeadingValue interpretation fails");

    bool result = false;
    if (is_available(a) && is_available(b)) {
        using vanetza::units::degree;
        const Angle angle_a { a.headingValue / 10.0 * degree };
        const Angle angle_b { b.headingValue / 10.0 * degree };
        result = similar_heading(angle_a, angle_b, limit);
    }

    return result;
}

bool similar_heading(const Heading& a, Angle b, Angle limit)
{
    bool result = false;
    if (is_available(a)) {
        using vanetza::units::degree;
        result = similar_heading(Angle { a.headingValue / 10.0 * degree}, b, limit);
    }
    return result;
}

bool similar_heading(Angle a, Angle b, Angle limit)
{
    using namespace boost::units;
    using boost::math::double_constants::pi;

    static const Angle full_circle = 2.0 * pi * si::radian;
    const Angle abs_diff = fmod(abs(a - b), full_circle);
    return abs_diff <= limit || abs_diff >= full_circle - limit;
}

units::Length distance(const ReferencePosition_t& a, const ReferencePosition_t& b)
{
    using geonet::GeodeticPosition;
    using units::GeoAngle;
    using boost::units::si::micro;
    using boost::units::degree::degree;

    auto length = units::Length::from_value(std::numeric_limits<double>::quiet_NaN());
    if (is_available(a) && is_available(b)) {
        GeodeticPosition geo_a {
            GeoAngle { a.latitude / Latitude_oneMicrodegreeNorth * micro * degree },
            GeoAngle { a.longitude / Longitude_oneMicrodegreeEast * micro * degree }
        };
        GeodeticPosition geo_b {
            GeoAngle { b.latitude / Latitude_oneMicrodegreeNorth * micro * degree },
            GeoAngle { b.longitude / Longitude_oneMicrodegreeEast * micro * degree }
        };
        length = geonet::distance(geo_a, geo_b);
    }
    return length;
}

units::Length distance(const ReferencePosition_t& a, units::GeoAngle lat, units::GeoAngle lon)
{
    using geonet::GeodeticPosition;
    using units::GeoAngle;
    using boost::units::si::micro;
    using boost::units::degree::degree;

    auto length = units::Length::from_value(std::numeric_limits<double>::quiet_NaN());
    if (is_available(a)) {
        GeodeticPosition geo_a {
            GeoAngle { a.latitude / Latitude_oneMicrodegreeNorth * micro * degree },
            GeoAngle { a.longitude / Longitude_oneMicrodegreeEast * micro * degree }
        };
        GeodeticPosition geo_b { lat, lon };
        length = geonet::distance(geo_a, geo_b);
    }
    return length;
}

bool is_available(const Heading& hd)
{
    return hd.headingValue != HeadingValue_unavailable;
}

bool is_available(const ReferencePosition& pos)
{
    return pos.latitude != Latitude_unavailable && pos.longitude != Longitude_unavailable;
}

} // namespace facilities
} // namespace vanetza

#endif /* CAM_FUNCTIONS_CPP_JQXFKSJP */
