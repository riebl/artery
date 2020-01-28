#ifndef ARTERY_ROS_SCHEDULER_H_T4WD6OCQ
#define ARTERY_ROS_SCHEDULER_H_T4WD6OCQ

#include <omnetpp/cscheduler.h>
#include <ros/ros.h>

namespace artery
{

class RosScheduler : public omnetpp::cScheduler
{
    public:
        RosScheduler();
        virtual ~RosScheduler();

        std::string str() const override;

        omnetpp::cEvent* guessNextEvent() override;
        omnetpp::cEvent* takeNextEvent() override;
        void putBackEvent(omnetpp::cEvent*) override;

        void endRun() override;
        void startRun() override;
        void executionResumed() override;

    protected:
        omnetpp::cEvent* peekFirstNonStaleEvent();
        void onTimer(const ros::SteadyTimerEvent&);

    private:
        ros::Time m_epoch;
        ros::WallTime m_reference;
        ros::Publisher m_clock;
        ros::SteadyTimer m_timer;
};

} // namespace artery

#endif /* ARTERY_ROS_SCHEDULER_H_T4WD6OCQ */
