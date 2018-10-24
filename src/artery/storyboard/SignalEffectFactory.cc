#include "artery/storyboard/SignalEffectFactory.h"
#include "artery/storyboard/SignalEffect.h"

namespace artery
{

std::shared_ptr<Effect> SignalEffectFactory::create(Vehicle& car, Story& story, ConditionResult& result)
{
    std::shared_ptr<Effect> ptr(new SignalEffect(car, mTriggerCause, story, result));
    return ptr;
}

} // namespace artery
