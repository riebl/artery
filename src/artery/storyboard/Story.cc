#include "artery/storyboard/Story.h"
#include "artery/storyboard/Storyboard.h"

Story::Story(Condition* condition, std::vector<EffectFactory*> factories) :
    m_condition(condition), m_factories(factories)
{
}

ConditionResult Story::testCondition(const Vehicle& car)
{
    return (m_condition->testCondition(car));
}

std::vector<EffectFactory*> Story::getEffectFactories() {
    return m_factories;
}
