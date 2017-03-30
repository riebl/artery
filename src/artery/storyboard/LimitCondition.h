#ifndef LIMITCONDITION_H_3PHIGCXK
#define LIMITCONDITION_H_3PHIGCXK

#include "artery/storyboard/Condition.h"
#include <set>

/**
 * Condition holds only true for the first N vehicles
 */
class LimitCondition : public Condition
{
public:
    LimitCondition(unsigned);

    ConditionResult testCondition(const Vehicle& car) override;

private:
    const unsigned m_limit;
    std::set<const Vehicle*> m_count;
};

#endif /* LIMITCONDITION_H_3PHIGCXK */

