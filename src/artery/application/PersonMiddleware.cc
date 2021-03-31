/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2021 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/StationType.h"
#include "artery/application/PersonMiddleware.h"
#include "artery/utility/InitStages.h"
#include "inet/common/ModuleAccess.h"

using namespace omnetpp;

namespace artery
{

Define_Module(PersonMiddleware)

void PersonMiddleware::initialize(int stage)
{
    if (stage == InitStages::Self) {
        mMobility = inet::getModuleFromPar<PersonMobility>(par("mobilityModule"), findHost());
        setStationType(vanetza::geonet::StationType::Pedestrian);
        getFacilities().register_const(mMobility);

        Identity identity;
        identity.traci = mMobility->getPersonId();
        identity.application = Identity::randomStationId(getRNG(0));
        emit(Identity::changeSignal, Identity::ChangeTraCI | Identity::ChangeStationId, &identity);
    }

    Middleware::initialize(stage);
}

} // namespace artery

