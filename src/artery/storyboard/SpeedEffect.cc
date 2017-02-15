#include "artery/storyboard/SpeedEffect.h"
#include "artery/traci/VehicleController.h"
#include <boost/units/io.hpp>
#include <omnetpp/clog.h>

void SpeedEffect::applyEffect()
{
    m_current = getCar()->getMaxSpeed() / boost::units::si::meter_per_second;
    getCar()->setMaxSpeed(m_speed * boost::units::si::meter_per_second);

    EV_STATICCONTEXT;
    EV_DEBUG << "SpeedEffect applied to " << getCar()->getVehicleId() << ": MaxSpeed = " << getCar()->getMaxSpeed() << "\n";
}

void SpeedEffect::removeEffect()
{
    getCar()->setMaxSpeed(m_current * boost::units::si::meter_per_second);

    EV_STATICCONTEXT;
    EV_DEBUG << "SpeedEffect removed from " << getCar()->getVehicleId() << ": MaxSpeed = " << getCar()->getMaxSpeed() << "\n";
}

void SpeedEffect::reapplyEffect()
{
    applyEffect();
}
