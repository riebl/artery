#ifndef TIMECONDITION_H_
#define TIMECONDITION_H_

namespace Veins {
    class TraCIMobility;
}

/**
 * Condition: Time
 * Checks if the current SimTime is in the specified time interval
 */
class TimeCondition : public Condition
{
public:

    /**
     * Creates a time interval between begin and end
     * begin and end are given in SimTime seconds, limits included
     */
    TimeCondition(SimTime begin, SimTime end) :
        m_begin(begin), m_end(end)
    {
    }

    /**
     * Creates a time interval between begin and the end of the simulation
     * begin and end are given in SimTime seconds, limits included
     */
    TimeCondition(SimTime begin) :
        m_begin(begin), m_end(SimTime::getMaxTime())
    {
    }

    /**
     * Returns True if the current SimTime is in the given interval
     * \param TraciMobility to test
     * \return result of the performed test
     */
    bool testCondition(Veins::TraCIMobility* car);

private:
    SimTime m_begin = 0;
    SimTime m_end = 0;
};

#endif
