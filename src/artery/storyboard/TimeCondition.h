#ifndef ARTERY_TIMECONDITION_H_
#define ARTERY_TIMECONDITION_H_

#include "artery/storyboard/Condition.h"
#include <omnetpp/simtime.h>

namespace artery
{

/**
 * Condition: Time
 * Checks if the current SimTime is in the specified time interval
 */
class STORYBOARD_API TimeCondition : public Condition
{
public:
    /**
     * Creates a time interval between begin and end
     * begin and end are given in SimTime seconds, limits included
     */
    TimeCondition(omnetpp::SimTime begin, omnetpp::SimTime end) :
        m_begin(begin), m_end(end)
    {
    }

    /**
     * Creates a time interval between begin and the end of the simulation
     * begin and end are given in SimTime seconds, limits included
     */
    TimeCondition(omnetpp::SimTime begin) :
        m_begin(begin), m_end(omnetpp::SimTime::getMaxTime())
    {
    }

    /**
     * Returns True if the current SimTime is in the given interval
     * \param TraciMobility to test
     * \return result of the performed test
     */
    ConditionResult testCondition(const Vehicle& car);

private:
    omnetpp::SimTime m_begin;
    omnetpp::SimTime m_end;
};

} // namespace artery

#endif
