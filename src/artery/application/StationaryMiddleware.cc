/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2018 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/StationaryMiddleware.h"
#include "artery/utility/InitStages.h"
#include "inet/common/ModuleAccess.h"
#include "traci/Core.h"
#include "inet/mobility/contract/IMobility.h"

namespace artery
{

Define_Module(StationaryMiddleware)

StationaryMiddleware::StationaryMiddleware() : mLocalDynamicMap(mTimer)
{
}

void StationaryMiddleware::initialize(int stage)
{
    if (stage == InitStages::Self) {
        setStationType(vanetza::geonet::StationType::RSU);
        getFacilities().register_mutable(&mLocalDynamicMap);
        getFacilities().register_mutable(&mVehicleDataProvider);

        inet::IMobility* mobility = inet::getModuleFromPar<inet::IMobility>(par("mobilityModule"), inet::getContainingNode(this));
        mVehicleDataProvider.setPosition(Position(mobility->getCurrentPosition().x, mobility->getCurrentPosition().y));

        traci::Core* core = check_and_cast<traci::Core*>(getModuleByPath(par("coreModule")));
        mVehicleDataProvider.computeGeoPosition(&core->getLiteAPI());
        mVehicleDataProvider.setStationType(vanetza::geonet::StationType::RSU);
    }

    Middleware::initialize(stage);
}

void StationaryMiddleware::handleMessage(cMessage *msg)
{
    if (msg == mUpdateMessage) {
        updateServices();
    } else {
        error("StationaryMiddleware cannot handle message '%s'", msg->getFullName());
    }
}

void StationaryMiddleware::updateServices()
{
    mLocalDynamicMap.dropExpired();
    Middleware::updateServices();
}

} // namespace artery
