#ifndef SPEEDEFFECT_H_
#define SPEEDEFFECT_H_

#include "artery/storyboard/Effect.h"

/**
 * SpeedEffect
 * Changes the cars speed according to m_speed
 * Resets the speed to the previous with removing the Effect
 */
class SpeedEffect : public Effect
{
public:
    /**
     * Creates a new Speed Effect
     * \param car Affected vehicle
     * \param Speed in m/s
     * \param Story from which the Effect is created
     */
    SpeedEffect(Vehicle& car, double speed, Story& story) :
        Effect(story, car), m_speed(speed)
    {
    }

    /**
     * Saves the current speed in m_current and changes the speed of m_car according to m_speed
     */
    void applyEffect();

    /**
     * Changes back m_cars speed to m_current
     */
    void removeEffect();

    /*
     * Calls apply Effect
     */
    void reapplyEffect();

private:
    double m_speed;
    double m_current;
};

#endif /* SPEEDEFFECT_H_ */
