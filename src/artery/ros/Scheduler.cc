#include "artery/ros/Scheduler.h"
#include <omnetpp/cevent.h>
#include <omnetpp/cfutureeventset.h>
#include <omnetpp/cmodule.h>
#include <omnetpp/regmacros.h>
#include <ros/callback_queue.h>
#include <ros/node_handle.h>
#include <rosgraph_msgs/Clock.h>
#include <map>
#include <string>

namespace artery
{

using namespace omnetpp;
Register_Class(RosScheduler)

RosScheduler::RosScheduler()
{
    // TODO read remappings from XML or omnetpp.ini
    std::map<std::string, std::string> remappings;
    ros::init(remappings, "artery", ros::init_options::NoSigintHandler);
    ros::start();

    ros::NodeHandle nh;
    nh.setParam("/use_sim_time", true);
    m_clock = nh.advertise<rosgraph_msgs::Clock>("/clock", 100, true);
    m_timer = nh.createSteadyTimer(ros::WallDuration(0.01),
            boost::bind(&RosScheduler::onTimer, this, boost::placeholders::_1),
            false, false);
}

RosScheduler::~RosScheduler()
{
    ros::shutdown();
}

std::string RosScheduler::str() const
{
    return "artery::ros::RosScheduler";
}

cEvent* RosScheduler::guessNextEvent()
{
    return sim->getFES()->peekFirst();
}

cEvent* RosScheduler::takeNextEvent()
{
    cEvent* event = peekFirstNonStaleEvent();
    ros::CallbackQueue* queue = ros::getGlobalCallbackQueue();

    ros::WallTime arrival = m_reference + ros::WallDuration { event->getArrivalTime().dbl() };
    ros::WallTime now = ros::WallTime::now();
    if (arrival > now) {
        queue->callAvailable(arrival - now);
    } else {
        queue->callAvailable();
    }

    cEvent* next = sim->getFES()->removeFirst();
    ASSERT(next == event);
    return event;
}

cEvent* RosScheduler::peekFirstNonStaleEvent()
{
    omnetpp::cEvent* event = nullptr;
    do {
        event = sim->getFES()->peekFirst();
        if (!event) {
            throw cTerminationException(E_ENDEDOK);
        } else if (event->isStale()) {
            event = nullptr;
            delete sim->getFES()->removeFirst();
        }
    } while (!event);

    return event;
}

void RosScheduler::putBackEvent(cEvent* event)
{
    sim->getFES()->putBackFirst(event);
}

void RosScheduler::startRun()
{
    m_reference = ros::WallTime::now();
    m_epoch.fromNSec(m_reference.toNSec());
    m_timer.start();
}

void RosScheduler::endRun()
{
    m_timer.stop();
}

void RosScheduler::executionResumed()
{
    m_reference = ros::WallTime::now();
    m_reference -= ros::WallDuration { simTime().dbl() };
}

void RosScheduler::onTimer(const ros::SteadyTimerEvent&)
{
    rosgraph_msgs::Clock now;
    now.clock = m_epoch + ros::Duration { simTime().dbl() };
    m_clock.publish(now);
}

} // namespace artery
