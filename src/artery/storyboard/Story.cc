#include "artery/storyboard/Story.h"
#include "artery/storyboard/Storyboard.h"

namespace artery
{

Story::Story(std::shared_ptr<Condition> condition, EffectFactories factories) :
    m_condition(condition), m_factories(factories)
{
}

ConditionResult Story::testCondition(const Vehicle& car)
{
    return (m_condition->testCondition(car));
}

auto Story::getEffectFactories() -> EffectFactories {
    return m_factories;
}

} // namespace artery
