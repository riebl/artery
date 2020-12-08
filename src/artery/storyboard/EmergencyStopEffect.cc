#include "artery/storyboard/EmergencyStopEffect.h"
#include "artery/storyboard/Vehicle.h"
#include "artery/traci/VehicleController.h"

namespace artery
{

void EmergencyStopEffect::applyEffect()
{
    traci::VehicleController& controller = getCar().getController();
    traci::LiteAPI& api = controller.getLiteAPI();
    api.vehicle().setSpeedMode(controller.getVehicleId(), 0);

    double speed = api.vehicle().getSpeed(controller.getVehicleId());
    double decel = api.vehicletype().getEmergencyDecel(controller.getTypeId());
    if (speed > 0.0 && decel > 0.0) {
        api.vehicle().slowDown(controller.getVehicleId(), 0.0, speed / decel);
    } else {
        controller.setSpeed(0.0 * boost::units::si::meter_per_second);
    }
}

void EmergencyStopEffect::reapplyEffect()
{
    applyEffect();
}

void EmergencyStopEffect::removeEffect()
{
}

} // namespace artery
