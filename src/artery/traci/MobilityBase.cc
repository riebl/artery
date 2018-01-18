#include "artery/traci/MobilityBase.h"
#include <omnetpp/ccomponent.h>

using namespace traci;

omnetpp::simsignal_t MobilityBase::stateChangedSignal = omnetpp::cComponent::registerSignal("mobilityStateChanged");

void MobilityBase::initializeSink(LiteAPI* api, const std::string& id, const TraCIBoundary& boundary)
{
    ASSERT(api);
    mTraci = api;
    mVehicleId= id;
    mNetBoundary = boundary;
    mController.reset(new VehicleController(id, *api));
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

