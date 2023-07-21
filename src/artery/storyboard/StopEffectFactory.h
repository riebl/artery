#ifndef ARTERY_STOPEFFECTFACTORY_H_
#define ARTERY_STOPEFFECTFACTORY_H_

#include "artery/storyboard/EffectFactory.h"
#include "artery/traci/VehicleController.h"

namespace artery
{

/**
 * StopEffectFactories creates StopEffects
 */
class STORYBOARD_API StopEffectFactory : public EffectFactory
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

} // namespace artery

#endif /* ARTERY_STOPEFFECTFACTORY_H_ */
