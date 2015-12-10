#ifndef ANDCONDITION_H_
#define ANDCONDITION_H_

#include "artery/storyboard/Condition.h"

/**
 * Class to create a Tree of AndConditions
 */
class AndCondition : public Condition
{
public:
    AndCondition(Condition*, Condition*);

    /**
     * Tests if the condition is true for one car
     * \param Car to test
     * \return true if both condition tests are passed
     */
    bool testCondition(const Vehicle& car);

private:
    Condition* m_left;
    Condition* m_right;
};

#endif /* ANDCONDITION_H_ */
