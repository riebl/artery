#include "artery/storyboard/StopEffect.h"
#include "artery/storyboard/StopEffectFactory.h"

std::shared_ptr<Effect> StopEffectFactory::create(Vehicle& car, Story& story, ConditionResult& resutl)
{
    return std::make_shared<StopEffect>(car, story);
}
