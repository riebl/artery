#include "artery/storyboard/SpeedEffect.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"

void SpeedEffect::applyEffect()
{
    m_current = getCar()->getVehicleCommandInterface()->getMaxSpeed();
    getCar()->getVehicleCommandInterface()->setMaxSpeed(m_speed);
    EV << "SpeedEffect applied: " << getCar()->getExternalId() << ": MaxSpeed set to: " << getCar()->getVehicleCommandInterface()->getMaxSpeed() << "\n";
}

void SpeedEffect::removeEffect()
{
    getCar()->getVehicleCommandInterface()->setMaxSpeed(m_current);
    EV << "SpeedEffect removed: " << getCar()->getExternalId() << ": MaxSpeed set to: " << getCar()->getVehicleCommandInterface()->getMaxSpeed() << "\n";
}

void SpeedEffect::reapplyEffect()
{
    applyEffect();
}
