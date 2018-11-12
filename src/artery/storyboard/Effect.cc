#include "artery/storyboard/Effect.h"

namespace artery
{

Story& Effect::getStory()
{
    return m_story;
}

Vehicle& Effect::getCar()
{
    return m_car;
}

Effect::Effect(Story& story, Vehicle& car) :
    m_story(story), m_car(car)
{
}

} // namespace artery
