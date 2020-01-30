#ifndef ARTERY_ROS_PROXYSERVICE_H_PNI671CK
#define ARTERY_ROS_PROXYSERVICE_H_PNI671CK

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

protected:
    const VehicleDataProvider* mVehicleDataProvider = nullptr;
    ros::Publisher mNavSatPublisher;
};

} // namespace artery

#endif /* ARTERY_ROS_PROXYSERVICE_H_PNI671CK */

