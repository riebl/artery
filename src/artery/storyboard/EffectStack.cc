#include <algorithm>
#include <boost/range/adaptor/reversed.hpp>
#include "artery/storyboard/EffectStack.h"
#include "artery/storyboard/Effect.h"
#include "artery/storyboard/Story.h"

namespace artery
{

void EffectStack::addEffect(std::shared_ptr<Effect> effect)
{
    if (std::count(m_effects.begin(), m_effects.end(), effect) == 0) {
        effect->applyEffect();
        m_effects.push_back(std::move(effect));
    }
}

void EffectStack::removeFromMiddle(const Effect* effect)
{
    // Loop through the vector
    // Count the removed Effects
    // If the wanted Effect is found, break
    unsigned removedEffects = 0;
    for (auto ef = m_effects.rbegin(); ef != m_effects.rend(); ef++) {
        (*ef)->removeEffect();
        if (ef->get() == effect) {
            m_effects.erase(--ef.base());
            break;
        }
        removedEffects++;
    }
    // Apply the removed Effects again
    unsigned effectCount = 0;
    for (auto ef : m_effects) {
        if(effectCount >= m_effects.size() - removedEffects) {
            ef->reapplyEffect();
        }
    }
}

void EffectStack::removeEffect(const Effect* effect)
{
    // If the effect is at the last position it can easily removed
    // else it have to be removed from the middle of the stack
    if (!m_effects.empty() && effect == m_effects.back().get()) {
        m_effects.back()->removeEffect();
        m_effects.pop_back();
    }
    else {
        removeFromMiddle(effect);
    }
}

void EffectStack::removeEffectsByStory(const Story* story)
{
    std::vector<Effect*> tmp;
    // Add all Effects related to a Story in a tmp vector
    for (auto effect : m_effects) {
        if (&effect->getStory() == story) {
            tmp.push_back(effect.get());
        }
    }

    // Remove all found Effects
    for (auto& effect : tmp) {
        removeEffect(effect);
    }
}

bool EffectStack::isStoryOnStack(const Story* story)
{
    for (auto effect : m_effects) {
        if (&effect->getStory() == story) {
            return true;
        }
    }
    return false;
}

} // namespace artery
