#include "traci/BasicModuleMapper.h"
#include <omnetpp/ccomponenttype.h>

using namespace omnetpp;

namespace traci
{

Define_Module(BasicModuleMapper)

void BasicModuleMapper::initialize()
{
    m_type = cModuleType::get(par("vehicleType"));
}

cModuleType* BasicModuleMapper::vehicle(NodeManager& manager, const std::string& id)
{
    return m_type;
}

} // namespace traci
