#include "artery/storyboard/SpeedEffect.h"
#include "artery/storyboard/Vehicle.h"
#include "artery/traci/VehicleController.h"
#include <boost/units/io.hpp>
#include <omnetpp/clog.h>

namespace artery
{

void SpeedEffect::applyEffect()
{
    auto& controller = getCar().getController();
    m_current = controller.getMaxSpeed() / boost::units::si::meter_per_second;
    controller.setMaxSpeed(m_speed * boost::units::si::meter_per_second);

    EV_STATICCONTEXT;
    EV_DEBUG << "SpeedEffect applied to " << controller.getVehicleId() << ": MaxSpeed = " << controller.getMaxSpeed() << "\n";
}

void SpeedEffect::removeEffect()
{
    auto& controller = getCar().getController();
    controller.setMaxSpeed(m_current * boost::units::si::meter_per_second);

    EV_STATICCONTEXT;
    EV_DEBUG << "SpeedEffect removed from " << controller.getVehicleId() << ": MaxSpeed = " << controller.getMaxSpeed() << "\n";
}

void SpeedEffect::reapplyEffect()
{
    applyEffect();
}

} // namespace artery
