#include "artery/storyboard/TimeCondition.h"
#include <omnetpp/csimulation.h>

namespace artery
{

ConditionResult TimeCondition::testCondition(const Vehicle& car)
{
    using namespace omnetpp;
    return (simTime() >= m_begin && simTime() <= m_end);
}

} // namespace artery
