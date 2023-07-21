#ifndef ARTERY_EFFECTSTACK_H_
#define ARTERY_EFFECTSTACK_H_

#include "artery/storyboard/Macros.h"
#include <vector>
#include <memory>

namespace artery
{

class Effect;
class Story;

class STORYBOARD_API EffectStack
{
public:
    /**
     * Adds an Effect to the Stack
     * \param Effect to add
     */
    void addEffect(std::shared_ptr<Effect>);

    /**
     * Removes an Effect, independent of it's position on the Stack
     * \param Effect to remove
     */
    void removeEffect(const Effect*);

    /**
     * Removes all effect related to a specific Story
     * \param Story to remove
     */
    void removeEffectsByStory(const Story*);

    /**
     * Checks if a Story is already added to the Stack
     * \param Story to check
     */
    bool isStoryOnStack(const Story*);

private:
    std::vector<std::shared_ptr<Effect>> m_effects;

    /**
     * Removes an Effect, that is not on top of the Stack
     * \param Effect to remove
     */
    void removeFromMiddle(const Effect*);
};

} // namespace artery

#endif /* ARTERY_EFFECTSTACK_H_ */
