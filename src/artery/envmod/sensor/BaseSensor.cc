/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/envmod/GlobalEnvironmentModel.h"
#include "artery/envmod/LocalEnvironmentModel.h"
#include "artery/envmod/sensor/BaseSensor.h"
#include "artery/application/Middleware.h"
#include "artery/application/Facilities.h"
#include "artery/utility/Identity.h"
#include <inet/common/ModuleAccess.h>
#include <cassert>

namespace artery
{

BaseSensor::BaseSensor() :
    mMiddleware(nullptr)
{
}

Facilities& BaseSensor::getFacilities()
{
    return getMiddleware().getFacilities();
}

const Facilities& BaseSensor::getFacilities() const
{
    return getMiddleware().getFacilities();
}

Middleware& BaseSensor::getMiddleware()
{
    assert(mMiddleware);
    return *mMiddleware;
}

const Middleware& BaseSensor::getMiddleware() const
{
    assert(mMiddleware);
    return *mMiddleware;
}

cModule* BaseSensor::findHost()
{
    return inet::getContainingNode(this);
}

void BaseSensor::initialize()
{
    cModule* module = findHost()->getSubmodule("middleware");
    Middleware* middleware = dynamic_cast<Middleware*>(module);
    if (middleware == nullptr) {
        throw cRuntimeError("Middleware not found");
    }

    mMiddleware = middleware;
    mLocalEnvironmentModel = mMiddleware->getFacilities().get_mutable_ptr<LocalEnvironmentModel>();
    mGlobalEnvironmentModel = mMiddleware->getFacilities().get_mutable_ptr<GlobalEnvironmentModel>();
}

std::string BaseSensor::getEgoId()
{
    return getFacilities().get_const<Identity>().traci;
}

} // namespace artery
