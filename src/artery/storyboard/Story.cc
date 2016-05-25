#include "artery/storyboard/Story.h"
#include "artery/storyboard/Storyboard.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"


Story::Story(std::vector<Condition*> conditions, std::vector<EffectFactory*> factories) :
    m_conditions(conditions), m_factories(factories)
{
}

bool Story::testConditions(Veins::TraCIMobility* car)
{
    bool conditionsPassed = true;
    for (auto cond : m_conditions) {
        if (!cond->testCondition(car)) {
            conditionsPassed = false;
        }
    }
    return conditionsPassed;
}

void Story::addCondition(Condition* con)
{
    m_conditions.push_back(con);
}

std::vector<EffectFactory*> Story::getEffectFactories() {
    return m_factories;
}
