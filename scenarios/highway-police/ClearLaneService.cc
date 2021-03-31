#include "ClearLaneService.h"
#include "police_msgs/PoliceClearLane_m.h"
#include "artery/traci/VehicleController.h"

using namespace omnetpp;
using namespace vanetza;

Define_Module(ClearLaneService)

void ClearLaneService::initialize()
{
    ItsG5BaseService::initialize();
    mVehicleController = &getFacilities().get_mutable<traci::VehicleController>();
}

void ClearLaneService::indicate(const vanetza::btp::DataIndication& ind, omnetpp::cPacket* packet)
{
    Enter_Method("ClearLaneService indicate");
    auto clearLaneMessage = check_and_cast<const PoliceClearLane*>(packet);

    const std::string id = mVehicleController->getVehicleId();
    auto& vehicle_api = mVehicleController->getTraCI()->vehicle;
    if (vehicle_api.getRoadID(id) == clearLaneMessage->getEdgeName()) {
        if (vehicle_api.getLaneIndex(id) != clearLaneMessage->getLaneIndex()) {
            slowDown();
        }
    }

    delete clearLaneMessage;
}

void ClearLaneService::slowDown()
{
    mVehicleController->setMaxSpeed(25 * units::si::meter_per_second);
}
