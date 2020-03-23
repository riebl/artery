#include "ots/UniformGtuCreationPolicy.h"
#include <omnetpp/ccomponenttype.h>

namespace ots
{

Define_Module(UniformGtuCreationPolicy)

void UniformGtuCreationPolicy::initialize()
{
    // one instruction to rule them all ;-)
    m_instruction.setModuleType(omnetpp::cModuleType::get(par("moduleType")));
    m_instruction.setSinkPath(par("gtuSinkPath"));
}

GtuCreationPolicy::Instruction UniformGtuCreationPolicy::getInstruction(const GtuObject&)
{
    return m_instruction;
}

} // namespace ots
