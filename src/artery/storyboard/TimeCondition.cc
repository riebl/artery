#include "artery/storyboard/Condition.h"
#include "artery/storyboard/Storyboard.h"
#include "artery/storyboard/TimeCondition.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"

bool TimeCondition::testCondition(const Vehicle& car)
{
    return (simTime() >= m_begin && simTime() <= m_end);
}
