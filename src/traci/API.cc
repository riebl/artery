#include "API.h"

#include <libsumo/Simulation.h>
#include <libsumo/TraCIConstants.h>
#include <omnetpp/cexception.h>

using namespace artery;

traci::TraCIGeoPosition::TraCIGeoPosition(double longitude, double latitude) : longitude(longitude), latitude(latitude)
{
}

traci::TraCIGeoPosition::TraCIGeoPosition(const lib::TraCIPosition& position)
{
    if (position.getType() != lib::POSITION_2D) {
        return;
    }

    lib::TraCIPosition result = lib::Simulation::convertGeo(position.x, position.y, false);
    longitude = result.x;
    latitude = result.y;
}

traci::lib::TraCIPosition traci::TraCIGeoPosition::convert2D()
{
    return lib::Simulation::convertGeo(longitude, latitude, true);
}

void traci::API::connectTraCI(const ServerEndpoint& endpoint)
{
#ifdef LIBTRACI
    std::size_t numRetries = (endpoint.retry) ? compat::DEFAULT_NUM_RETRIES : 0;
    compat::Simulation::init(endpoint.port, numRetries, endpoint.hostname);
    compat::Simulation::setOrder(endpoint.clientId);
#else
    throw omnetpp::cRuntimeError("libsumo does not allow connecting to existing endpoint. Please compile with LIBTRACI instead");
#endif
}
