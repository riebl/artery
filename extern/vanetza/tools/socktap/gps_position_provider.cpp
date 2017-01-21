#include "gps_position_provider.hpp"
#include <vanetza/units/angle.hpp>
#include <vanetza/units/velocity.hpp>
#include <cmath>

static_assert(GPSD_API_MAJOR_VERSION == 6, "libgps has incompatible API");

GpsPositionProvider::GpsPositionProvider()
{
    if (gps_open(GPSD_SHARED_MEMORY, nullptr, &gps_data)) {
        throw gps_error(errno);
    }
    gps_stream(&gps_data, WATCH_ENABLE | WATCH_JSON, nullptr);
}

GpsPositionProvider::~GpsPositionProvider()
{
    gps_stream(&gps_data, WATCH_DISABLE, nullptr);
    gps_close(&gps_data);
}

GpsPositionProvider::gps_error::gps_error(int err) :
    std::runtime_error(gps_errstr(err))
{
}

vanetza::geonet::LongPositionVector GpsPositionProvider::current_position()
{
    if (gps_read(&gps_data) < 0) {
        throw gps_error(errno);
    }

    vanetza::geonet::LongPositionVector pv;
    pv.position_accuracy_indicator = false;
    if (gps_data.status == STATUS_FIX && gps_data.fix.mode >= MODE_2D) {
        using geo_angle_t = vanetza::geonet::geo_angle_i32t;
        using speed_t = vanetza::geonet::LongPositionVector::speed_u15t;
        using heading_t = vanetza::geonet::heading_u16t;
        using namespace vanetza::units;

        pv.timestamp = convert(gps_data.fix.time);
        pv.position_accuracy_indicator = true; // TODO check position error
        pv.latitude = static_cast<geo_angle_t>(gps_data.fix.latitude * degree);
        pv.longitude = static_cast<geo_angle_t>(gps_data.fix.longitude * degree);
        pv.speed = static_cast<speed_t>(gps_data.fix.speed * si::meter_per_second);
        pv.heading = static_cast<heading_t>(gps_data.fix.track * degree);
    }

    return pv;
}

vanetza::geonet::Timestamp GpsPositionProvider::convert(timestamp_t gpstime) const
{
    using namespace boost::gregorian;
    using namespace boost::posix_time;

    // gpsd's timestamp_t is UNIX time (UTC) with fractional seconds
    static date posix_epoch(1970, Jan, 1);
    timestamp_t gps_integral;
    timestamp_t gps_fractional = std::modf(gpstime, &gps_integral);
    auto posix_seconds = seconds(gps_integral);
    auto posix_milliseconds = milliseconds(gps_fractional * 1000.0);
    ptime posix_time { posix_epoch, posix_seconds + posix_milliseconds };

    // TAI has some seconds bias compared to UTC
    const auto tai_utc_bias = seconds(36); // 36 seconds since 1st July 2015
    return vanetza::geonet::Timestamp { posix_time + tai_utc_bias };
}
