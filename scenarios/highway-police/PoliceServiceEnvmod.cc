#include "PoliceServiceEnvmod.h"
#include "artery/envmod/LocalEnvironmentModel.h"

using namespace omnetpp;

Define_Module(PoliceServiceEnvmod)

void PoliceServiceEnvmod::initialize()
{
    PoliceService::initialize();
    localEnvmod = &getFacilities().get_const<artery::LocalEnvironmentModel>();
}

void PoliceServiceEnvmod::trigger()
{
    Enter_Method("PoliceServiceEnvmod trigger");
    const auto& objects = localEnvmod->allObjects();
    if (objects.size() > 0) {
        PoliceService::trigger();
    }
}
