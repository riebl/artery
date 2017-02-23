#include "artery/traci/MobilityBase.h"

using namespace traci;


void MobilityBase::initializeVehicle(LiteAPI* api, const std::string& id, const TraCIBoundary& boundary)
{
    ASSERT(api);
    mTraci = api;
    mVehicleId= id;
    mNetBoundary = boundary;
    mController.reset(new VehicleController(id, *api));
}

void MobilityBase::updateVehicle(const TraCIPosition& traci_pos, TraCIAngle traci_heading, double traci_speed)
{
    const auto opp_pos = position_cast(mNetBoundary, traci_pos);
    const auto opp_angle = angle_cast(traci_heading);
    update(opp_pos, opp_angle, traci_speed);
}

VehicleController* MobilityBase::getVehicleController()
{
    ASSERT(mController);
    return mController.get();
}

