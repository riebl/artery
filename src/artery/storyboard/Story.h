#ifndef STORY_H_
#define STORY_H_

#include <vector>
#include "artery/storyboard/Condition.h"
#include "artery/storyboard/EffectFactory.h"

namespace Veins
{
class TraCIMobility;
}
class Storyboard;
class EffectFactory;

/**
 * Class providing a basic set of member functions for creating a story
 * Can be used to create stories with any trigger conditions and any effect
 */
class Story
{
public:
    Story(std::vector<Condition*>, std::vector<EffectFactory*>);

    /**
     * Tests all cars if the conditions in m_condidions are passed or not
     * Invokes adding effects to cars which have passed the condition
     * param: std::vector<Veins::TraCIMobility*> containing all cars in the simulation
     */
    bool testConditions(Veins::TraCIMobility*);

    /**
     * Adds a Condition to an existing Story
     * param: Condition* to add
     */
    void addCondition(Condition*);

    /**
     * Returns vector containing all EffectFactories
     */
    std::vector<EffectFactory*> getEffectFactories();

private:
    std::vector<Condition*> m_conditions;
    std::vector<EffectFactory*> m_factories;
};

#endif /* STORY_H_ */
