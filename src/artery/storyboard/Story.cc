#include "artery/storyboard/Story.h"
#include "artery/storyboard/Storyboard.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"


Story::Story(Condition* condition, std::vector<EffectFactory*> factories) :
    m_condition(condition), m_factories(factories)
{
}

bool Story::testCondition(Veins::TraCIMobility* car)
{
    return (m_condition->testCondition(car));
}

std::vector<EffectFactory*> Story::getEffectFactories() {
    return m_factories;
}
