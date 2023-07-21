#ifndef ARTERY_EFFECT_H_
#define ARTERY_EFFECT_H_

#include "artery/storyboard/Macros.h"

namespace artery
{

class Story;
class Vehicle;

/**
 * Effect Interface
 */
class STORYBOARD_API Effect
{
public:
    Effect(Story& story, Vehicle& car);
    virtual ~Effect() = default;
    virtual void applyEffect() = 0;
    virtual void reapplyEffect() = 0;
    virtual void removeEffect() = 0;
    Story& getStory();
    Vehicle& getCar();

private:
    Story& m_story;
    Vehicle& m_car;
};

} // namespace artery

#endif /* ARTERY_EFFECT_H_ */
