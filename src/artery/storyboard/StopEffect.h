#ifndef SRC_ARTERY_STORYBOARD_NODES_STOPEFFECT_H_
#define SRC_ARTERY_STORYBOARD_NODES_STOPEFFECT_H_

#include "artery/storyboard/Effect.h"

/**
 * StopEffect
 * Changes the current speed of the car to 0
 * This causes the car to slow down to zero (and starting the waiting timer in SUMO)
 * Speed is reset to previous speed after removing the effect
 */
class StopEffect : public Effect
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

#endif /* SRC_ARTERY_STORYBOARD_NODES_STOPEFFECT_H_ */
