#include "artery/application/VehicleKinematics.h"
#include "artery/traci/Controller.h"
#include <limits>

namespace artery
{

VehicleKinematics::VehicleKinematics() :
    acceleration(vanetza::units::Acceleration::from_value(std::numeric_limits<double>::quiet_NaN())),
    yaw_rate(vanetza::units::AngularVelocity::from_value(std::numeric_limits<double>::quiet_NaN()))
{
}

VehicleKinematics getKinematics(const traci::Controller& controller)
{
    artery::VehicleKinematics kinematics;
    kinematics.position = controller.getPosition();
    kinematics.geo_position = controller.getGeoPosition();
    kinematics.speed = controller.getSpeed();
    kinematics.heading = controller.getHeading().getTrueNorth();
    return kinematics;
}

} // namespace artery
