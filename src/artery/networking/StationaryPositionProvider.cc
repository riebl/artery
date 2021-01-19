#include "artery/networking/StationaryPositionProvider.h"
#include "artery/traci/Cast.h"
#include "traci/Core.h"
#include "traci/LiteAPI.h"
#include "traci/Position.h"
#include <inet/mobility/contract/IMobility.h>
#include <inet/common/ModuleAccess.h>
#include <veins/base/modules/BaseMobility.h>
#include <vanetza/common/runtime.hpp>

namespace artery
{

Define_Module(StationaryPositionProvider)

static const omnetpp::simsignal_t scPositionFixSignal = omnetpp::cComponent::registerSignal("PositionFix");

void StationaryPositionProvider::initialize(int stage)
{
    if (stage == 0) {
        Listener::subscribeTraCI(getSystemModule());
    }
}

void StationaryPositionProvider::traciInit()
{
    auto pos = getInitialPosition();
    initializePosition(pos);
}

Position StationaryPositionProvider::getInitialPosition()
{
    auto mobilityModule = inet::getModuleFromPar<omnetpp::cModule>(par("mobilityModule"), this);
    if (auto mobility = dynamic_cast<inet::IMobility*>(mobilityModule)) {
        inet::Coord inet_pos = mobility->getCurrentPosition();
        return Position { inet_pos.x, inet_pos.y };
    } else if (auto mobility = dynamic_cast<veins::BaseMobility*>(mobilityModule)) {
        veins::Coord veins_pos = mobility->getPositionAt(omnetpp::simTime());
        return Position { veins_pos.x, veins_pos.y };
    } else {
        error("no suitable mobility module found");
        return Position {}; // never reached
    }
}

void StationaryPositionProvider::initializePosition(const Position& pos)
{
    // TODO inet::IGeographicCoordinateSystem provided by TraCI module would be nice
    auto traci = inet::getModuleFromPar<traci::Core>(par("traciCoreModule"), this);
    traci::LiteAPI& api = traci->getLiteAPI();
    const traci::Boundary boundary { api.simulation().getNetBoundary() };
    traci::TraCIGeoPosition geopos = api.convertGeo(traci::position_cast(boundary, Position { pos.x, pos.y }));

    using namespace vanetza::units;
    mPositionFix.timestamp = inet::getModuleFromPar<vanetza::Runtime>(par("runtimeModule"), this)->now();
    mPositionFix.latitude = geopos.latitude * degree;
    mPositionFix.longitude = geopos.longitude * degree;
    mPositionFix.confidence.semi_minor = 1.0 * si::meter;
    mPositionFix.confidence.semi_major = 1.0 * si::meter;
    mPositionFix.course.assign(TrueNorth {}, TrueNorth {});
    mPositionFix.speed.assign(0.0 * si::meter_per_second, 0.0 * si::meter_per_second);

    // prevent signal listeners to modify our position data
    PositionFixObject tmp { mPositionFix };
    emit(scPositionFixSignal, &tmp);
}

} // namespace artery
