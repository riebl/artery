#ifndef ARTERY_LIMITCONDITION_H_3PHIGCXK
#define ARTERY_LIMITCONDITION_H_3PHIGCXK

#include "artery/storyboard/Condition.h"
#include <set>

namespace artery
{

/**
 * Condition holds only true for the first N vehicles
 */
class STORYBOARD_API LimitCondition : public Condition
{
public:
    LimitCondition(unsigned);

    ConditionResult testCondition(const Vehicle& car) override;

private:
    const unsigned m_limit;
    std::set<const Vehicle*> m_count;
};

} // namespace artery

#endif /* ARTERY_LIMITCONDITION_H_3PHIGCXK */

