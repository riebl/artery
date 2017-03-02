#include "artery/storyboard/StopEffect.h"
#include "artery/storyboard/StopEffectFactory.h"

std::shared_ptr<Effect> StopEffectFactory::create(traci::VehicleController& car, Story* story)
{
    return std::make_shared<StopEffect>(car, story);
}
