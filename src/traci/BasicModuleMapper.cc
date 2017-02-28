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
    m_type = cModuleType::get(par("vehicleType"));
    m_penetration = par("penetrationRate");
}

cModuleType* BasicModuleMapper::vehicle(NodeManager& manager, const std::string& id)
{
    return equipVehicle() ? m_type : nullptr;
}

bool BasicModuleMapper::equipVehicle()
{
    const double dice = omnetpp::uniform(m_rng, 0.0, 1.0);
    return dice < m_penetration;
}

} // namespace traci
