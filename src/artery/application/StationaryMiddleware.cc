/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2020 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/StationaryMiddleware.h"
#include "artery/utility/Geometry.h"
#include "artery/utility/InitStages.h"
#include <omnetpp/checkandcast.h>
#include <vanetza/common/position_fix.hpp>

namespace artery
{

using namespace omnetpp;
static const simsignal_t scPositionFixSignal = cComponent::registerSignal("PositionFix");

Define_Module(StationaryMiddleware)

void StationaryMiddleware::initialize(int stage)
{
    if (stage == InitStages::Self) {
        setStationType(vanetza::geonet::StationType::RSU);

        Identity identity;
        identity.application = Identity::deriveStationId(findHost(), par("stationIdDerivation").stringValue());
        emit(Identity::changeSignal, Identity::ChangeStationId, &identity);

        if (cModule* host = findHost()) {
            // position will be set by PositionFix signal
            host->subscribe(scPositionFixSignal, this);
            getFacilities().register_const(&mGeoPosition);
        }
    }

    Middleware::initialize(stage);
}

void StationaryMiddleware::receiveSignal(cComponent*, simsignal_t signal, cObject* obj, cObject*)
{
    if (signal == scPositionFixSignal) {
        auto position = check_and_cast<const vanetza::PositionFix*>(obj);
        mGeoPosition.latitude = position->latitude;
        mGeoPosition.longitude = position->longitude;
    }
}

} // namespace artery
