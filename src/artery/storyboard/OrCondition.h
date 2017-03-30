#ifndef ORCONDITION_H_
#define ORCONDITION_H_

#include "artery/storyboard/Condition.h"

/**
 * Class to create a Tree of OrConditions
 */
class OrCondition : public Condition
{
public:
    OrCondition(Condition*, Condition*);

    /**
     * Tests if the condition is true for one car
     * \param Car to test
     * \return true if condition test for left or right is passed
     */
    ConditionResult testCondition(const Vehicle& car) override;

    void drawCondition(omnetpp::cCanvas*) override;

private:
    Condition* m_left;
    Condition* m_right;
};

#endif /* ORCONDITION_H_ */
