#include "artery/storyboard/TimeCondition.h"
#include <omnetpp/csimulation.h>

bool TimeCondition::testCondition(const Vehicle& car)
{
    using namespace omnetpp;
    return (simTime() >= m_begin && simTime() <= m_end);
}
