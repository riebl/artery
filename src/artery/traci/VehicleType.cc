#include "artery/traci/VehicleType.h"
#include <boost/units/systems/si/acceleration.hpp>
#include <boost/units/systems/si/length.hpp>
#include <boost/units/systems/si/velocity.hpp>

namespace si = boost::units::si;

namespace traci
{

VehicleType::VehicleType(const std::string& type_id, traci::LiteAPI& api) :
    m_id(type_id), m_api(api.vehicletype())
{
}

const std::string& VehicleType::getTypeId()
{
    return m_id;
}

std::string VehicleType::getVehicleClass()
{
    return m_api.getVehicleClass(m_id);
}

auto VehicleType::getMaxSpeed() -> Velocity
{
    return m_api.getMaxSpeed(m_id) * si::meter_per_second;
}

auto VehicleType::getMaxAcceleration() -> Acceleration
{
    return m_api.getAccel(m_id) * si::meter_per_second_squared;
}

auto VehicleType::getMaxDeceleration() -> Acceleration
{
    return m_api.getDecel(m_id) * si::meter_per_second_squared;
}

auto VehicleType::getLength() -> Length
{
    return m_api.getLength(m_id) * si::meter;
}

auto VehicleType::getWidth() -> Length
{
    return m_api.getWidth(m_id) * si::meter;
}

auto VehicleType::getHeight() -> Length
{
    return m_api.getHeight(m_id) * si::meter;
}

} // namespace traci
