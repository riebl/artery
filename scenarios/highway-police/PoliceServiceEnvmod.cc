#include "PoliceServiceEnvmod.h"
#include "artery/envmod/LocalEnvironmentModel.h"

using namespace omnetpp;

Define_Module(PoliceServiceEnvmod)

void PoliceServiceEnvmod::initialize()
{
    PoliceService::initialize();
    localEnvmod = &getFacilities().get_const<artery::LocalEnvironmentModel>();
    skippedTrigger = 0;
}

void PoliceServiceEnvmod::trigger()
{
    Enter_Method("PoliceServiceEnvmod trigger");
    const auto& objects = localEnvmod->allObjects();
    if (objects.size() > 0 || skippedTrigger >= 4) {
        PoliceService::trigger();
        skippedTrigger = 0;
    } else {
        ++skippedTrigger;
    }
}
