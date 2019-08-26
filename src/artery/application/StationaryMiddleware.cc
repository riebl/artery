/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2018 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/StationaryMiddleware.h"
#include "artery/utility/InitStages.h"

namespace artery
{

Define_Module(StationaryMiddleware)

void StationaryMiddleware::initialize(int stage)
{
    if (stage == InitStages::Self) {
        setStationType(vanetza::geonet::StationType::RSU);
    }

    Middleware::initialize(stage);
}

} // namespace artery
