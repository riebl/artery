#ifndef SRC_ARTERY_STORYBOARD_STOPEFFECTFACTORY_H_
#define SRC_ARTERY_STORYBOARD_STOPEFFECTFACTORY_H_

#include "artery/storyboard/EffectFactory.h"
#include "artery/traci/VehicleController.h"

/**
 * StopEffectFactories creates StopEffects
 */
class StopEffectFactory : public EffectFactory
{
public:
    StopEffectFactory() {};

    /**
     * Creates a new Effect for a VehicleController
     * \param VehicleController which should be affected
     * \param Story from which the effect is created
     * \return Effect to add on the EffectStack
     */
    std::shared_ptr<Effect> create(Vehicle&, Story&, ConditionResult&) override;
};

#endif /* SRC_ARTERY_STORYBOARD_STOPEFFECTFACTORY_H_ */
