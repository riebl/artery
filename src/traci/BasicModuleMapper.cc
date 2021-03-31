#include "traci/BasicModuleMapper.h"
#include <omnetpp/ccomponenttype.h>
#include <omnetpp/distrib.h>

using namespace omnetpp;

namespace traci
{

Define_Module(BasicModuleMapper)

void BasicModuleMapper::initialize()
{
    m_rng = getRNG(0);
    m_person_type = cModuleType::find(par("personType"));
    m_vehicle_type = cModuleType::get(par("vehicleType"));

    double penetration = par("penetrationRate");
    m_person_penetration = par("personPenetrationRate");
    if (m_person_penetration < 0.0) {
        m_person_penetration = penetration;
    }
    m_vehicle_penetration = par("vehiclePenetrationRate");
    if (m_vehicle_penetration < 0.0) {
        m_vehicle_penetration = penetration;
    }
}

cModuleType* BasicModuleMapper::person(NodeManager& manager, const std::string& id)
{
    return (m_person_type && equipPerson()) ? m_person_type : nullptr;
}

cModuleType* BasicModuleMapper::vehicle(NodeManager& manager, const std::string& id)
{
    return equipVehicle() ? m_vehicle_type : nullptr;
}

bool BasicModuleMapper::equipPerson()
{
    const double dice = omnetpp::uniform(m_rng, 0.0, 1.0);
    return dice < m_person_penetration;
}

bool BasicModuleMapper::equipVehicle()
{
    const double dice = omnetpp::uniform(m_rng, 0.0, 1.0);
    return dice < m_vehicle_penetration;
}

} // namespace traci
