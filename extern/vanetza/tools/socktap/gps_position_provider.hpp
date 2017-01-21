#ifndef GPS_POSITION_PROVIDER_HPP_GYN3GVQA
#define GPS_POSITION_PROVIDER_HPP_GYN3GVQA

#include "position_provider.hpp"
#include <stdexcept>
#include <gps.h>

class GpsPositionProvider : public PositionProvider
{
public:
    class gps_error : public std::runtime_error
    {
    protected:
        gps_error(int);
        friend class GpsPositionProvider;
    };

    GpsPositionProvider();
    ~GpsPositionProvider();
    vanetza::geonet::LongPositionVector current_position();

private:
    vanetza::geonet::Timestamp convert(timestamp_t) const;

    gps_data_t gps_data;
};

#endif /* GPS_POSITION_PROVIDER_HPP_GYN3GVQA */

