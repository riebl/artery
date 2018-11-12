/*
 * Artery V2X Simulation Framework
 * Copyright 2017,2018 Thiago Vieira, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/inet/gemv2/LinkClassifier.h"
#include "artery/inet/gemv2/ObstacleIndex.h"
#include "artery/inet/gemv2/VehicleIndex.h"
#include <inet/common/ModuleAccess.h>

namespace artery
{
namespace gemv2
{

Define_Module(LinkClassifier)

void LinkClassifier::initialize()
{
    mObstacleIndex = inet::findModuleFromPar<ObstacleIndex>(par("obstacleIndexModule"), this);
    mFoliageIndex = inet::findModuleFromPar<ObstacleIndex>(par("foliageIndexModule"), this);
    mVehicleIndex = inet::findModuleFromPar<VehicleIndex>(par("vehicleIndexModule"), this);

    WATCH(mCountLOS);
    WATCH(mCountNLOSb);
    WATCH(mCountNLOSf);
    WATCH(mCountNLOSv);
}

void LinkClassifier::finish()
{
    recordScalar("countLOS", mCountLOS);
    recordScalar("countNLOSb", mCountNLOSb);
    recordScalar("countNLOSf", mCountNLOSf);
    recordScalar("countNLOSv", mCountNLOSv);
}

LinkClass LinkClassifier::classifyLink(const Position& tx, const Position& rx) const
{
    LinkClass link = LinkClass::LOS;
    if (mObstacleIndex->anyBlockage(tx, rx)) {
        link = LinkClass::NLOSb;
        ++mCountNLOSb;
    } else if (mFoliageIndex->anyBlockage(tx, rx)) {
        link = LinkClass::NLOSf;
        ++mCountNLOSf;
    } else if (mVehicleIndex->anyBlockage(tx, rx)) {
        link = LinkClass::NLOSv;
        ++mCountNLOSv;
    } else {
        ++mCountLOS;
    }
    return link;
}

} // namespace gemv2
} // namespace artery
