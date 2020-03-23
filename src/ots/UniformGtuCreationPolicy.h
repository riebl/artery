#ifndef OTS_UNIFORMGTUCREATIONPOLICY_H_7IYR1XMH
#define OTS_UNIFORMGTUCREATIONPOLICY_H_7IYR1XMH

#include "ots/GtuCreationPolicy.h"
#include <omnetpp/csimplemodule.h>

namespace ots
{

class UniformGtuCreationPolicy :
    public GtuCreationPolicy, public omnetpp::cSimpleModule
{
public:
    void initialize() override;
    Instruction getInstruction(const GtuObject&) override;

private:
    Instruction m_instruction;
};

} // namespace ots

#endif /* OTS_UNIFORMGTUCREATIONPOLICY_H_7IYR1XMH */
