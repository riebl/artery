#include <libsumo/Simulation.h>
#include <libsumo/TraCIConstants.h>

#include "Defs.h"

using namespace artery;

traci::TraCIGeoPosition::TraCIGeoPosition(double longitude, double latitude)
    : longitude(longitude)
    , latitude(latitude)
{}

traci::TraCIGeoPosition::TraCIGeoPosition(const compat::TraCIPosition& position) {
    if (position.getType() != compat::POSITION_2D) {
        return;
    }

    compat::TraCIPosition result = compat::Simulation::convertGeo(position.x, position.y, false);
    longitude = result.x;
    latitude = result.y;
}

traci::compat::TraCIPosition traci::TraCIGeoPosition::convert2D() {
    return compat::Simulation::convertGeo(longitude, latitude, true);
}
