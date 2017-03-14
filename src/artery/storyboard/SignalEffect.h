#ifndef SIGNALEFFECT_H
#define SIGNALEFFECT_H

#include "artery/storyboard/Effect.h"
#include "artery/storyboard/Condition.h"
#include "omnetpp/cobject.h"

/*
 * SignalEffect
 * Triggers an OMNeT Signal which can be received by any submodule of car
 * Signal is triggered only if the effect is applied the first time
 */
class SignalEffect : public Effect
{
public:
    SignalEffect(traci::VehicleController& car, std::string cause, Story* story, ConditionResult& result) :
        Effect(story, car), mTriggerCause(cause), mResult(result)
    {
    }

    /**
     * Triggers a signal containing the triggerCause, the conditionResult, the VehicleController and the emitting car
     */
    void applyEffect() override;

    /**
     * Nothing to do here
     */
    void removeEffect() override
    {
    }

    /**
     * Nothing to do here
     */
    void reapplyEffect() override
    {
    }

private:
    std::string mTriggerCause;
    ConditionResult& mResult;
};

struct StoryboardSignal : public omnetpp::cObject
{
    ConditionResult result;
    traci::VehicleController* car;
    std::string cause;
    Story* story;
};

#endif /* SIGNALEFFECT_H */
