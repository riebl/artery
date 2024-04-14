#include "artery/traci/Controller.h"
#include "artery/traci/Cast.h"
#include "traci/VariableCache.h"
#include <boost/units/systems/angle/degrees.hpp>
#include <boost/units/systems/si/acceleration.hpp>
#include <boost/units/systems/si/plane_angle.hpp>
#include <boost/units/systems/si/velocity.hpp>

namespace si = boost::units::si;

namespace traci
{

Controller::Controller(std::shared_ptr<traci::API> api, std::shared_ptr<VariableCache> cache) :
    m_traci(api),
    m_boundary(api->simulation.getNetBoundary()),
    m_cache(cache)
{
}

const std::string& Controller::getId() const
{
    return m_cache->getId();
}

const std::shared_ptr<VariableCache>& Controller::getCache() const
{
    return m_cache;
}

const traci::Boundary& Controller::getBoundary() const
{
    return m_boundary;
}

std::string Controller::getTypeId() const
{
    return m_cache->get<libsumo::VAR_TYPE>();

}

artery::Position Controller::getPosition() const
{
    return traci::position_cast(m_boundary, m_cache->get<libsumo::VAR_POSITION>());
}

auto Controller::getGeoPosition() const -> artery::GeoPosition
{
    TraCIPosition traci_pos = m_cache->get<libsumo::VAR_POSITION>();

    TraCIGeoPosition traci_geo = m_traci->convertGeo(traci_pos);
    artery::GeoPosition geo;
    geo.latitude = traci_geo.latitude * boost::units::degree::degree;
    geo.longitude = traci_geo.longitude * boost::units::degree::degree;
    return geo;
}

auto Controller::getHeading() const -> artery::Angle
{
    using namespace traci;
    return angle_cast(TraCIAngle { m_cache->get<libsumo::VAR_ANGLE>() });
}

auto Controller::getSpeed() const -> Velocity
{
    return m_cache->get<libsumo::VAR_SPEED>() * si::meter_per_second;
}

auto Controller::getLength() const -> Length
{
    return m_cache->get<libsumo::VAR_LENGTH>() * si::meter;
}

auto Controller::getWidth() const -> Length
{
    return m_cache->get<libsumo::VAR_WIDTH>() * si::meter;
}

} // namespace traci
