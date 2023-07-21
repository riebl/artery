#ifndef ARTERY_ANDCONDITION_H_
#define ARTERY_ANDCONDITION_H_

#include "artery/storyboard/Condition.h"
#include <algorithm>
#include <memory>

namespace artery
{

/**
 * Class to create a Tree of AndConditions
 */
class STORYBOARD_API AndCondition : public Condition
{
public:
    using ConditionPtr = std::shared_ptr<Condition>;

    AndCondition(ConditionPtr, ConditionPtr);

    /**
     * Tests if the condition is true for one car
     * \param Car to test
     * \return true if both condition tests are passed
     */
    ConditionResult testCondition(const Vehicle& car) override;

    void drawCondition(omnetpp::cCanvas*) override;

private:
    ConditionPtr m_left;
    ConditionPtr m_right;
};

} // namespace artery

#endif /* ARTERY_ANDCONDITION_H_ */
