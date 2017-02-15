#include "artery/storyboard/Effect.h"


Story* Effect::getStory()
{
    return m_story;
}

traci::VehicleController* Effect::getCar()
{
    return m_car;
}

Effect::Effect(Story* story, traci::VehicleController& car) :
    m_story(story), m_car(&car)
{
}
