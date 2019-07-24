#include "artery/traci/VehicleController.h"
#include "traci/VariableCache.h"
#include <boost/units/systems/angle/degrees.hpp>
#include <boost/units/systems/si/acceleration.hpp>
#include <boost/units/systems/si/plane_angle.hpp>
#include <boost/units/systems/si/velocity.hpp>

namespace si = boost::units::si;

namespace traci
{

VehicleController::VehicleController(const std::string& id, traci::LiteAPI& api) :
    VehicleController(id, api, std::make_shared<VehicleCache>(api, id))
{
}

VehicleController::VehicleController(std::shared_ptr<VehicleCache> cache) :
    VehicleController(cache->getVehicleId(), cache->getLiteAPI(), cache)
{
}

VehicleController::VehicleController(const std::string& id, traci::LiteAPI& api, std::shared_ptr<VehicleCache> cache) :
    m_id(id), m_api(api), m_boundary(api.simulation().getNetBoundary()),
    m_type(api.vehicle().getTypeID(id), api), m_cache(cache)
{
}

const std::string& VehicleController::getVehicleId() const
{
    return m_id;
}

std::string VehicleController::getTypeId() const
{
    return m_cache->get<libsumo::VAR_TYPE>();

}

const VehicleType& VehicleController::getVehicleType() const
{
    return m_type;
}

const std::string VehicleController::getVehicleClass() const
{
    return m_cache->get<libsumo::VAR_VEHICLECLASS>();
}

artery::Position VehicleController::getPosition() const
{
    return traci::position_cast(m_boundary, m_cache->get<libsumo::VAR_POSITION>());
}

auto VehicleController::getGeoPosition() const -> artery::GeoPosition
{
    TraCIPosition traci_pos = m_cache->get<libsumo::VAR_POSITION>();

    TraCIGeoPosition traci_geo = m_api.convertGeo(traci_pos);
    artery::GeoPosition geo;
    geo.latitude = traci_geo.latitude * boost::units::degree::degree;
    geo.longitude = traci_geo.longitude * boost::units::degree::degree;
    return geo;
}

auto VehicleController::getHeading() const -> artery::Angle
{
    using namespace traci;
    return angle_cast(TraCIAngle { m_cache->get<libsumo::VAR_ANGLE>() });
}

auto VehicleController::getSpeed() const -> Velocity
{
    return m_cache->get<libsumo::VAR_SPEED>() * si::meter_per_second;
}

auto VehicleController::getMaxSpeed() const -> Velocity
{
    return m_cache->get<libsumo::VAR_MAXSPEED>() * si::meter_per_second;
}

void VehicleController::setMaxSpeed(Velocity v)
{
    m_api.vehicle().setMaxSpeed(m_id, v / si::meter_per_second);
}

void VehicleController::setSpeed(Velocity v)
{
    m_api.vehicle().setSpeed(m_id, v / si::meter_per_second);
}

void VehicleController::setSpeedFactor(double f)
{
    m_api.vehicle().setSpeedFactor(m_id, f);
}

auto VehicleController::getLength() const -> Length
{
    return m_cache->get<libsumo::VAR_LENGTH>() * si::meter;
}

auto VehicleController::getWidth() const -> Length
{
    return m_cache->get<libsumo::VAR_WIDTH>() * si::meter;
}

void VehicleController::changeTarget(const std::string& edge)
{
    m_api.vehicle().changeTarget(m_id, edge);
}

} // namespace traci
