#include "artery/application/StoryboardSignal.h"
#include <omnetpp.h>

namespace artery
{

Register_Class(StoryboardSignal)

StoryboardSignal::StoryboardSignal() :
    mEgo(nullptr), mStory(nullptr)
{
}

void StoryboardSignal::setEgoVehicle(Vehicle* v)
{
    mEgo = v;
}

const Vehicle* StoryboardSignal::getEgoVehicle() const
{
    return mEgo;
}

void StoryboardSignal::setOtherVehicles(const std::set<const Vehicle*>& others)
{
    mOthers = others;
}

const std::set<const Vehicle*> StoryboardSignal::getOtherVehicles() const
{
    return mOthers;
}

void StoryboardSignal::setCause(const std::string& cause)
{
    mCause = cause;
}

const std::string& StoryboardSignal::getCause() const
{
    return mCause;
}

void StoryboardSignal::setStory(Story* s)
{
    mStory = s;
}

const Story* StoryboardSignal::getStory() const
{
    return mStory;
}

omnetpp::cObject* StoryboardSignal::dup() const
{
    return new StoryboardSignal(*this);
}

} // namespace artery
