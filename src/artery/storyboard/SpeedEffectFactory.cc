#include <artery/storyboard/SpeedEffectFactory.h>
#include <artery/storyboard/SpeedEffect.h>

std::shared_ptr<Effect> SpeedEffectFactory::create(Veins::TraCIMobility* car, Story* story)
{
    std::shared_ptr<Effect> ptr(new SpeedEffect(car, m_speed, story));
    return ptr;
}
