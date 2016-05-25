#include "artery/storyboard/Effect.h"


Story* Effect::getStory()
{
    return m_story;
}

Veins::TraCIMobility* Effect::getCar()
{
    return m_car;
}

Effect::Effect(Story* story, Veins::TraCIMobility* car) :
    m_story(story), m_car(car)
{
}
