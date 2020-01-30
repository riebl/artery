#include "artery/ros/ProxyService.h"
#include "artery/application/VehicleDataProvider.h"
#include "artery/utility/Identity.h"
#include <sensor_msgs/NavSatFix.h>
#include <regex>

namespace artery
{

Define_Module(RosProxyService)

void RosProxyService::initialize()
{
    ItsG5PromiscuousService::initialize();
    mVehicleDataProvider = getFacilities().get_const_ptr<VehicleDataProvider>();

    std::string ns = par("namespace");

    // replace %SUMO_ID% placeholder by actual ID maintained by SUMO
    const Identity& identity = getFacilities().get_const<Identity>();
    ns = std::regex_replace(ns, std::regex("%SUMO_ID%"), identity.traci);

    // replace characters not allowed in ROS names
    std::regex ros_name("[^a-zA-Z0-9/]");
    ns = std::regex_replace(ns, ros_name, "_");

    ros::NodeHandle nh(ns);
    mNavSatPublisher = nh.advertise<sensor_msgs::NavSatFix>("nav_sat_fix", 10);
}

void RosProxyService::trigger()
{
    Enter_Method("trigger");
    if (mVehicleDataProvider) {
        sensor_msgs::NavSatFix fix;
        fix.header.stamp = ros::Time::now();
        fix.status.status = sensor_msgs::NavSatStatus::STATUS_FIX;
        fix.status.service = sensor_msgs::NavSatStatus::SERVICE_GPS;
        fix.latitude = mVehicleDataProvider->latitude() / vanetza::units::degree;
        fix.longitude = mVehicleDataProvider->longitude() / vanetza::units::degree;
        fix.position_covariance_type = sensor_msgs::NavSatFix::COVARIANCE_TYPE_UNKNOWN;
        mNavSatPublisher.publish(fix);
    }
}

} // namespace artery
