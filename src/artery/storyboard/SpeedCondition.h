#ifndef SPEEDCONDITION_H_
#define SPEEDCONDITION_H_

#include "artery/storyboard/Condition.h"
#include "artery/application/ItsG5Middleware.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"
#include <functional>
#include <omnetpp.h>

template<typename COMP>
class SpeedCondition : public Condition {
public:
    SpeedCondition(double speed) :
        m_speed(speed * boost::units::si::meter_per_second), m_comp()
    {
    }

    bool testCondition(const Vehicle& car)
    {
        return m_comp(car.vdp.speed(), m_speed);
    }

private:
    vanetza::units::Velocity m_speed;
    const COMP m_comp;
};

#endif /* SPEEDCONDITION_H_ */
