#ifndef VEHICLETYPE_H_QHTSUY2F
#define VEHICLETYPE_H_QHTSUY2F

#include "traci/LiteAPI.h"
#include <vanetza/units/acceleration.hpp>
#include <vanetza/units/angle.hpp>
#include <vanetza/units/length.hpp>
#include <vanetza/units/velocity.hpp>
#include <string>

namespace traci
{

class VehicleType
{
public:
    using Acceleration = vanetza::units::Acceleration;
    using Length = vanetza::units::Length;
    using Velocity = vanetza::units::Velocity;

    VehicleType(const std::string& type_id, traci::LiteAPI&);

    const std::string& getTypeId();
    std::string getVehicleClass();
    Velocity getMaxSpeed();
    Acceleration getMaxAcceleration();
    Acceleration getMaxDeceleration();
    Length getLength();
    Length getWidth();
    Length getHeight();

private:
    std::string m_id;
    traci::API::VehicleTypeScope& m_api;
};

} // namespace traci

#endif /* VEHICLETYPE_H_QHTSUY2F */

