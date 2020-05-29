#include "artery/networking/Runtime.h"
#include <omnetpp/cmessage.h>

using vanetza::Clock;

namespace artery
{

Define_Module(Runtime)

Runtime::~Runtime()
{
    cancelAndDelete(mUpdateEvent);
}

void Runtime::initialize(int stage)
{
    if (stage == 0) {
        mUpdateEvent = new omnetpp::cMessage("runtime update");
        mTimer.setTimebase(par("datetime"));
        mRuntime.reset(mTimer.getCurrentTime());
        mLastUpdate = omnetpp::simTime();
    }
}

void Runtime::handleMessage(omnetpp::cMessage* msg)
{
    if (msg == mUpdateEvent) {
        update();
        schedule();
    }
}

void Runtime::schedule(Clock::time_point tp, const Callback& cb, const void* scope)
{
    Enter_Method("schedule");
    mRuntime.schedule(tp, cb, scope);
    schedule();
}

void Runtime::schedule(Clock::duration d, const Callback& cb, const void* scope)
{
    Enter_Method("schedule");
    mRuntime.schedule(d, cb, scope);
    schedule();
}

void Runtime::cancel(const void* scope)
{
    Enter_Method("cancel");
    mRuntime.cancel(scope);
    schedule();
}

Clock::time_point Runtime::now() const
{
    if (mLastUpdate != omnetpp::simTime()) {
        // updating runtime's current time here will not trigger any callbacks
        ASSERT(mRuntime.next() == Clock::time_point::max() || convertSimTime(mRuntime.next()) > omnetpp::simTime());
        // hence, const_cast is just a little bit of cheating
        const_cast<Runtime*>(this)->update();
    }
    return mRuntime.now();
}

void Runtime::update()
{
    mLastUpdate = omnetpp::simTime();
    mRuntime.trigger(mTimer.getCurrentTime());
}

void Runtime::schedule()
{
    auto next_event = mRuntime.next();
    while (next_event < vanetza::Clock::time_point::max()) {
        if (next_event > mRuntime.now()) {
            cancelEvent(mUpdateEvent);
            scheduleAt(convertSimTime(next_event), mUpdateEvent);
            break;
        } else {
            mRuntime.trigger(mRuntime.now());
            next_event = mRuntime.next();
        }
    }
}

omnetpp::SimTime Runtime::convertSimTime(vanetza::Clock::time_point tp) const
{
    using namespace std::chrono;
    using namespace omnetpp;
    const auto d = duration_cast<microseconds>(tp - mRuntime.now());
    return SimTime { simTime().inUnit(SIMTIME_US) + d.count(), SIMTIME_US };
}

} // namespace artery
