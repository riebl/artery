/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/envmod/sensor/CamSensor.h"
#include "artery/envmod/GlobalEnvironmentModel.h"
#include "artery/envmod/LocalEnvironmentModel.h"
#include "artery/application/CaObject.h"
#include "artery/application/Middleware.h"
#include "artery/utility/IdentityRegistry.h"
#include <inet/common/ModuleAccess.h>

using namespace omnetpp;

namespace artery
{

static const simsignal_t CamReceivedSignal = cComponent::registerSignal("CamReceived");

Define_Module(CamSensor)

void CamSensor::initialize()
{
    BaseSensor::initialize();
    mIdentityRegistry = inet::getModuleFromPar<IdentityRegistry>(par("identityRegistryModule"), this);
    getMiddleware().subscribe(CamReceivedSignal, this);
}

void CamSensor::finish()
{
    getMiddleware().unsubscribe(CamReceivedSignal, this);
    BaseSensor::finish();
}

void CamSensor::measurement()
{
    Enter_Method("measurement");
}

void CamSensor::receiveSignal(cComponent*, simsignal_t signal, cObject *obj, cObject*)
{
    if (signal == CamReceivedSignal) {
        auto* cam = dynamic_cast<CaObject*>(obj);
        if (cam) {
            uint32_t stationID = cam->asn1()->header.stationID;
            auto identity = mIdentityRegistry->lookup<IdentityRegistry::application>(stationID);
            if (identity) {
                auto object = mGlobalEnvironmentModel->getObject(identity->traci);
                SensorDetection detection;
                detection.objects.push_back(object);
                mLocalEnvironmentModel->complementObjects(detection, *this);
            } else {
                EV_WARN << "Unknown identity for station ID " << stationID;
            }
        } else {
            EV_ERROR << "received signal has no CaObject";
        }
    }
}

omnetpp::SimTime CamSensor::getValidityPeriod() const
{
    return omnetpp::SimTime { 1100, SIMTIME_MS };
}

const std::string& CamSensor::getSensorCategory() const
{
    static const std::string category = "CA";
    return category;
}

} // namespace artery
