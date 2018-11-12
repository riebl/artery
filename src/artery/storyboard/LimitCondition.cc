#include "artery/storyboard/LimitCondition.h"

namespace artery
{

LimitCondition::LimitCondition(unsigned limit) :
    m_limit(limit)
{
}

ConditionResult LimitCondition::testCondition(const Vehicle& v)
{
    bool condition = false;
    if (m_count.find(&v) != m_count.end()) {
        condition = true;
    } else if (m_count.size() < m_limit) {
        m_count.insert(&v);
        condition = true;
    }
    return condition;
}

} // namespace artery
