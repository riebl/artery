#include "artery/ros/ProxyService.h"
#include "artery/application/VehicleDataProvider.h"

namespace artery
{

Define_Module(RosProxyService)

void RosProxyService::initialize()
{
    ItsG5PromiscuousService::initialize();
    mVehicleDataProvider = &getFacilities().get_const<VehicleDataProvider>();
}

void RosProxyService::trigger()
{
    Enter_Method("trigger");
}

} // namespace artery
