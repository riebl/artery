#include <artery/storyboard/SpeedEffectFactory.h>
#include <artery/storyboard/SpeedEffect.h>

std::shared_ptr<Effect> SpeedEffectFactory::create(traci::VehicleController& car, Story* story, ConditionResult& result)
{
    std::shared_ptr<Effect> ptr(new SpeedEffect(car, m_speed, story));
    return ptr;
}
