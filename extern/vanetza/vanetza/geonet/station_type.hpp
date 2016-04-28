#ifndef STATION_TYPE_HPP_BARYX6ET
#define STATION_TYPE_HPP_BARYX6ET

namespace vanetza
{
namespace geonet
{

enum class StationType {
    UNKNOWN = 0,
    PEDESTRIAN = 1,
    CYCLIST = 2,
    MOPED = 3,
    MOTORCYCLE = 4,
    PASSENGER_CAR = 5,
    BUS = 6,
    LIGHT_TRUCK = 7,
    HEAVY_TRUCK = 8,
    TRAILER = 9,
    SPECIAL_VEHICLE = 10,
    TRAM = 11,
    RSU = 15
};

} // namespace geonet
} // namespace vanetza

#endif /* STATION_TYPE_HPP_BARYX6ET */

