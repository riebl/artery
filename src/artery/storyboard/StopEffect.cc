#include "artery/storyboard/StopEffect.h"
#include "artery/storyboard/Vehicle.h"
#include "artery/traci/VehicleController.h"

void StopEffect::applyEffect()
{
    getCar().getController().setSpeed(0.0 * boost::units::si::meter_per_second);
}

void StopEffect::reapplyEffect()
{
    applyEffect();
}

void StopEffect::removeEffect()
{
    getCar().getController().setSpeed(-1.0 * boost::units::si::meter_per_second);
}
