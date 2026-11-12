#include "SionnaVehicleActor.h"

#include <artery/sionna/environment/Compat.h>

using namespace artery::sionna;

Define_Module(SionnaVehicleActor);

void SionnaVehicleActor::initialize(int stage)
{
    InetVehicleMobility::initialize(stage);
}

void SionnaVehicleActor::finish()
{
    removeSionnaObject(mVehicleId);
    InetVehicleMobility::finish();
}

const std::string& SionnaVehicleActor::sumoId() const
{
    return mVehicleId;
}

void SionnaVehicleActor::initializeVehicle(const traci::TraCIPosition& position, traci::TraCIAngle heading, double speed)
{
    initializeSionnaActor(this);
    object() = makeSceneObject(mVehicleId);
    auto transform = api()->dynamicConfiguration()->addObject(object());
    updateSionnaObject(transform.get(), position, heading, false);
    VehicleMobility::initializeVehicle(position, heading, speed);
}

void SionnaVehicleActor::updateVehicle(const traci::TraCIPosition& position, traci::TraCIAngle heading, double speed)
{
    auto transform = api()->dynamicConfiguration()->updateObject(sceneId(mVehicleId));
    updateSionnaObject(transform.get(), position, heading);
    VehicleMobility::updateVehicle(position, heading, speed);
}
