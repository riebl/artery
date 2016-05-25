#ifndef EFFECTSTACK_H_
#define EFFECTSTACK_H_

#include <vector>
#include <memory>

class Effect;
class Story;

class EffectStack
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

#endif /* EFFECTSTACK_H_ */
