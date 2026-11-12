#pragma once

#include <libsumo/libsumo.h>

namespace artery {

namespace traci {

/**
 * @brief Namespace alias for currently used libsumo/libtraci. Use this to access common API.
 *
 */
namespace lib = LIBSUMO_NAMESPACE;

/**
 * @brief Geographical position.
 *
 * Geographical position that is defined by pair of values (longitude, latitude),
 * both measured in degrees. Note, 3D Traci positions cannot be converted to this class.
 */
class TraCIGeoPosition
{
public:
    TraCIGeoPosition() = default;
    TraCIGeoPosition(double longitude, double latitude);
    TraCIGeoPosition(const lib::TraCIPosition& position);

    lib::TraCIPosition convert2D();

    double longitude = lib::INVALID_DOUBLE_VALUE;
    double latitude = lib::INVALID_DOUBLE_VALUE;
};

/**
 * @brief Small helper metadata class for initializing connection to SUMO via libtraci.
 */
struct ServerEndpoint {
    std::string hostname;
    std::size_t port;
    std::size_t clientId = 1;
    bool retry = false;
};

/**
 * @brief TraCI angle
 * - measured in degree
 * - 0 is headed north
 * - clockwise orientation (i.e. 90 heads east)
 * - range from 0 to 360
 */
struct TraCIAngle {
    explicit TraCIAngle(double d) : degree(d) {}
    double degree;
};

class API : public libsumo::Simulation
{
public:
    using Version = std::pair<int, std::string>;

    /**
     * @brief Connect to SUMO instance with \Endpoint metadata.
     *
     * @param endpoint Connection details.
     */
    void connectTraCI(const ServerEndpoint& endpoint);
};

}  // namespace traci

}  // namespace artery
