#ifndef STORY_H_
#define STORY_H_

#include <vector>
#include "artery/storyboard/Condition.h"
#include "artery/storyboard/EffectFactory.h"
#include "artery/storyboard/Vehicle.h"

class Storyboard;
class EffectFactory;

/**
 * Class providing a basic set of member functions for creating a story
 * Can be used to create stories with any trigger conditions and any effect
 */
class Story
{
public:
    Story(Condition*, std::vector<EffectFactory*>);

    /**
     * Tests the car if the condition is true
     * \param car to test
     * \return result of the test
     */
    ConditionResult testCondition(const Vehicle&);

    /**
     * Returns vector containing all EffectFactories
     */
    std::vector<EffectFactory*> getEffectFactories();

    Condition* getCondition() { return m_condition; }

private:
    Condition* m_condition;
    std::vector<EffectFactory*> m_factories;
};

#endif /* STORY_H_ */
