#include "artery/storyboard/StopEffect.h"
#include "artery/storyboard/StopEffectFactory.h"

namespace artery
{

std::shared_ptr<Effect> StopEffectFactory::create(Vehicle& car, Story& story, ConditionResult& resutl)
{
    return std::make_shared<StopEffect>(car, story);
}

} // namespace artery
