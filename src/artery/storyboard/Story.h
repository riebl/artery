#ifndef ARTERY_STORY_H_
#define ARTERY_STORY_H_

#include "artery/storyboard/Condition.h"
#include "artery/storyboard/EffectFactory.h"
#include "artery/storyboard/Macros.h"
#include "artery/storyboard/Vehicle.h"
#include <memory>
#include <vector>

namespace artery
{

class Storyboard;
class EffectFactory;

/**
 * Class providing a basic set of member functions for creating a story
 * Can be used to create stories with any trigger conditions and any effect
 */
class STORYBOARD_API Story
{
public:
    using EffectFactories = std::vector<std::shared_ptr<EffectFactory>>;

    Story(std::shared_ptr<Condition>, EffectFactories);

    /**
     * Tests the car if the condition is true
     * \param car to test
     * \return result of the test
     */
    ConditionResult testCondition(const Vehicle&);

    /**
     * Returns vector containing all EffectFactories
     */
    std::vector<std::shared_ptr<EffectFactory>> getEffectFactories();

    std::shared_ptr<Condition> getCondition() { return m_condition; }

private:
    std::shared_ptr<Condition> m_condition;
    EffectFactories m_factories;
};

} // namespace artery

#endif /* ARTERY_STORY_H_ */
