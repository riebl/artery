#ifndef SPEEDEFFECTFACTORY_H_
#define SPEEDEFFECTFACTORY_H_

#include "artery/storyboard/Effect.h"
#include "artery/storyboard/EffectFactory.h"
#include "artery/traci/VehicleController.h"
#include <memory>

/**
 * SpeedEffectFactory creates SpeedEffects
 */
class SpeedEffectFactory : public EffectFactory
{
public:
    SpeedEffectFactory(double speed) :
        m_speed(speed)
    {
    }

    /**
     * Creates a new Effect for a TraCIMobility
     * \param Vehicle which should be affected from the Effect
     * \param Story from which the Effect should be created
     * \return Effect to add on the EffectStack from the TraCIMobility
     */
    std::shared_ptr<Effect> create(Vehicle&, Story&, ConditionResult&) override;

private:
    double m_speed;
};

#endif /* SPEEDEFECTFACTORY_H_ */
