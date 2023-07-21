#ifndef ARTERY_STOPEFFECT_H_
#define ARTERY_STOPEFFECT_H_

#include "artery/storyboard/Effect.h"

namespace artery
{

/**
 * StopEffect
 * Changes the current speed of the car to 0
 * This causes the car to slow down to zero (and starting the waiting timer in SUMO)
 * Speed is reset to previous speed after removing the effect
 */
class STORYBOARD_API StopEffect : public Effect
{
public:
    StopEffect(Vehicle& car, Story& story) :
        Effect(story, car)
    {
    }

    /**
     * Stops the car by changing its current speed to zero
     */
    void applyEffect();

    /**
     * Calls apply effect
     */
    void reapplyEffect();

    /**
     * Resets the behavior, car follows the car-following rules again
     */
    void removeEffect();
};

} // namespace artery

#endif /* ARTERY_STOPEFFECT_H_ */
