#ifndef ARTERY_SPEEDDIFFERENCECONDITION_H
#define ARTERY_SPEEDDIFFERENCECONDITION_H

#include "artery/storyboard/Condition.h"
#include "boost/units/quantity.hpp"
#include "boost/units/systems/si/velocity.hpp"

namespace artery
{

class STORYBOARD_API SpeedDifferenceCondition : public Condition
{
public:
    SpeedDifferenceCondition(double difference) :
        mSpeedDifference(difference * boost::units::si::meter_per_second)
    {
    }

    virtual ConditionResult testCondition(const Vehicle& car) = 0;

protected:
    const boost::units::quantity<boost::units::si::velocity> mSpeedDifference;
};


class STORYBOARD_API SpeedDifferenceConditionFaster : public SpeedDifferenceCondition
{
public:
    SpeedDifferenceConditionFaster(double difference) : SpeedDifferenceCondition(difference)
    {
    }

    virtual ConditionResult testCondition(const Vehicle& car) override;
};

class STORYBOARD_API SpeedDifferenceConditionSlower : public SpeedDifferenceCondition
{
public:
    SpeedDifferenceConditionSlower(double difference) : SpeedDifferenceCondition(difference)
    {
    }

    virtual ConditionResult testCondition(const Vehicle& car) override;
};

} // namespace artery

#endif /* ARTERY_SPEEDDIFFERENCECONDITION_H */
