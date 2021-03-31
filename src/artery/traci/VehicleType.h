#ifndef VEHICLETYPE_H_QHTSUY2F
#define VEHICLETYPE_H_QHTSUY2F

#include "traci/API.h"
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

    VehicleType(const traci::API::VehicleTypeScope&, const std::string& id);

    const std::string& getTypeId() const;
    std::string getVehicleClass() const;
    Velocity getMaxSpeed() const;
    Acceleration getMaxAcceleration() const;
    Acceleration getMaxDeceleration() const;
    Length getLength() const;
    Length getWidth() const;
    Length getHeight() const;

private:
    std::string m_id;
    const traci::API::VehicleTypeScope& m_api;
};

} // namespace traci

#endif /* VEHICLETYPE_H_QHTSUY2F */

