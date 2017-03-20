#ifndef SIGNALEFFECTFACTORY_H
#define SIGNALEFFECTFACTORY_H

#include "artery/storyboard/Effect.h"
#include "artery/storyboard/EffectFactory.h"
#include "artery/traci/VehicleController.h"

/**
 * SignalEffectFactory creates SignalEffects
 */
class SignalEffectFactory : public EffectFactory
{
public:
    SignalEffectFactory(std::string cause) :
        mTriggerCause(cause)
    {
    }

    /**
     * Creates a new Effect for a Vehicle
     * \param Vehicle which should be affected
     * \param Story from which the Effect should be created
     * \param ConditionResult result of the condition test
     * \return Effect to add on the EffectStack
     */
    std::shared_ptr<Effect> create(Vehicle&, Story&, ConditionResult&) override;

private:
    std::string mTriggerCause;
};

#endif /* SIGNALEFFECTFACTORY_H */
