#include "artery/storyboard/StopEffect.h"
#include "artery/traci/VehicleController.h"

void StopEffect::applyEffect()
{
    getCar()->setSpeed(0 * boost::units::si::meter_per_second);
}

void StopEffect::reapplyEffect()
{
    applyEffect();
}

void StopEffect::removeEffect()
{
    getCar()->setSpeed(-1 * boost::units::si::meter_per_second);
}
