#include "artery/traci/Cast.h"
#include "artery/traci/MobilityBase.h"
#include <omnetpp/ccomponent.h>

using namespace traci;

namespace artery
{

omnetpp::simsignal_t MobilityBase::stateChangedSignal = omnetpp::cComponent::registerSignal("mobilityStateChanged");

void MobilityBase::initializeSink(std::shared_ptr<API> api, std::shared_ptr<VehicleCache> cache, const Boundary& boundary)
{
    ASSERT(api);
    ASSERT(cache);
    mTraci = api;
    mVehicleId = cache->getId();
    mNetBoundary = boundary;
    mController.reset(new VehicleController(api, cache));
}

void MobilityBase::initializeVehicle(const TraCIPosition& traci_pos, TraCIAngle traci_heading, double traci_speed)
{
    const auto opp_pos = position_cast(mNetBoundary, traci_pos);
    const auto opp_angle = angle_cast(traci_heading);
    initialize(opp_pos, opp_angle, traci_speed);
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

} // namespace artery
