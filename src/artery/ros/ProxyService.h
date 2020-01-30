#ifndef ARTERY_ROS_PROXYSERVICE_H_PNI671CK
#define ARTERY_ROS_PROXYSERVICE_H_PNI671CK

#include "artery/application/CaObject.h"
#include "artery/application/ItsG5PromiscuousService.h"
#include <ros/ros.h>

namespace artery
{

class VehicleDataProvider;


class RosProxyService : public ItsG5PromiscuousService
{
public:
    void initialize() override;
    void trigger() override;
    void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, omnetpp::cObject*, omnetpp::cObject* = nullptr) override;

protected:
    void publishReceivedCam(const CaObject&);

    const VehicleDataProvider* mVehicleDataProvider = nullptr;
    ros::Publisher mNavSatPublisher;
    ros::Publisher mCamRxPublisher;
};

} // namespace artery

#endif /* ARTERY_ROS_PROXYSERVICE_H_PNI671CK */

