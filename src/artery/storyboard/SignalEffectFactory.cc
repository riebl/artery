#include "artery/storyboard/SignalEffectFactory.h"
#include "artery/storyboard/SignalEffect.h"

std::shared_ptr<Effect> SignalEffectFactory::create(Vehicle& car, Story& story, ConditionResult& result)
{
    std::shared_ptr<Effect> ptr(new SignalEffect(car, mTriggerCause, story, result));
    return ptr;
}
