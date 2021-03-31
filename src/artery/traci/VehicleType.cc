#include "artery/traci/VehicleType.h"
#include <boost/units/systems/si/acceleration.hpp>
#include <boost/units/systems/si/length.hpp>
#include <boost/units/systems/si/velocity.hpp>

namespace si = boost::units::si;

namespace traci
{

VehicleType::VehicleType(const traci::API::VehicleTypeScope& api, const std::string& id) :
    m_id(id), m_api(api)
{
}

const std::string& VehicleType::getTypeId() const
{
    return m_id;
}

std::string VehicleType::getVehicleClass() const
{
    return m_api.getVehicleClass(m_id);
}

auto VehicleType::getMaxSpeed() const -> Velocity
{
    return m_api.getMaxSpeed(m_id) * si::meter_per_second;
}

auto VehicleType::getMaxAcceleration() const -> Acceleration
{
    return m_api.getAccel(m_id) * si::meter_per_second_squared;
}

auto VehicleType::getMaxDeceleration() const -> Acceleration
{
    return m_api.getDecel(m_id) * si::meter_per_second_squared;
}

auto VehicleType::getLength() const -> Length
{
    return m_api.getLength(m_id) * si::meter;
}

auto VehicleType::getWidth() const -> Length
{
    return m_api.getWidth(m_id) * si::meter;
}

auto VehicleType::getHeight() const -> Length
{
    return m_api.getHeight(m_id) * si::meter;
}

} // namespace traci
