/*
 * Artery V2X Simulation Framework
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/envmod/sensor/CpmSensor.h"

#include "artery/application/Middleware.h"
#include "artery/application/cp/CpObject.h"
#include "artery/envmod/GlobalEnvironmentModel.h"
#include "artery/envmod/LocalEnvironmentModel.h"
#include "artery/utility/IdentityRegistry.h"

#include <inet/common/ModuleAccess.h>
#include <vanetza/asn1/its/CpmParameters.h>


namespace opp = omnetpp;


/**
 * Signals for inter-module communication
 */
namespace evt
{
static const auto cpmReceived = opp::cComponent::registerSignal("CpmReceived");
}  // namespace evt


namespace artery
{

Define_Module(CpmSensor)

void CpmSensor::initialize()
{
    mValidityPeriod = par("validityPeriod");
    BaseSensor::initialize();
    mIdentityRegistry = inet::getModuleFromPar<IdentityRegistry>(par("identityRegistryModule"), this);
    getMiddleware().subscribe(evt::cpmReceived, this);
}

void CpmSensor::finish()
{
    getMiddleware().unsubscribe(evt::cpmReceived, this);
    BaseSensor::finish();
}


void CpmSensor::measurement()
{
    Enter_Method("measurement");
}

void CpmSensor::receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t signal, omnetpp::cObject* obj, omnetpp::cObject*)
{
    EV_TRACE << "receiveSingal(...)\n";

    if (signal == evt::cpmReceived) {
        auto* cpm = dynamic_cast<CpObject*>(obj);
        if (cpm) {
            const auto& message = cpm->asn1();
            SensorDetection detection;

            auto identity = mIdentityRegistry->lookup<IdentityRegistry::application>(message->header.stationID);
            if (identity) {
                auto object = mGlobalEnvironmentModel->getObject(identity->traci);
                detection.objects.push_back(object);
            } else {
                EV_WARN << "Unknown identity for sender station ID " << message->header.stationID << "\n";
            }

            if (message->cpm.cpmParameters.perceivedObjectContainer) {
                const auto* perceivedObjects = message->cpm.cpmParameters.perceivedObjectContainer;
                for (int i = 0; i < perceivedObjects->list.count; ++i) {
                    const auto* obj = perceivedObjects->list.array[i];
                    const auto egoStationID = getFacilities().get_const<Identity>().application;
                    if (obj->objectID == egoStationID) {
                        // Don't add the ego vehicle to the local environment
                        continue;
                    }

                    identity = mIdentityRegistry->lookup<IdentityRegistry::application>(obj->objectID);
                    if (identity) {
                        auto object = mGlobalEnvironmentModel->getObject(identity->traci);
                        detection.objects.push_back(object);
                    } else {
                        EV_WARN << "Unknown identity for object station ID " << obj->objectID << "\n";
                    }
                }
            }

            mLocalEnvironmentModel->complementObjects(detection, *this);
        } else {
            EV_ERROR << "Received signal has no CpObject\n";
        }
    }
}

omnetpp::SimTime CpmSensor::getValidityPeriod() const
{
    return mValidityPeriod;
}

const std::string& CpmSensor::getSensorCategory() const
{
    static const std::string category = "CP";
    return category;
}

SensorDetection CpmSensor::detectObjects() const
{
    // return empty sensor detection because CPM objects are added upon CPM reception signal
    SensorDetection detection;
    return detection;
}

}  // namespace artery
