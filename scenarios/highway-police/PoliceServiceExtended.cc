#include "PoliceServiceExtended.h"
#include "police_msgs/PoliceClearLane_m.h"
#include "artery/traci/VehicleController.h"
#include <vanetza/btp/data_request.hpp>
#include <vanetza/dcc/profile.hpp>
#include <vanetza/geonet/interface.hpp>

using namespace omnetpp;
using namespace vanetza;

// Register Signal to recieve signal from Storyboard
static const simsignal_t storyboardSignal = cComponent::registerSignal("StoryboardSignal");

Define_Module(PoliceServiceExtended)

void PoliceServiceExtended::initialize()
{
    PoliceService::initialize();

    // Subscribe to Storyboard signal
    subscribe(storyboardSignal);
}

void PoliceServiceExtended::trigger()
{
    Enter_Method("PoliceServiceExtended trigger");

    if(isTriggered) {
        btp::DataRequestB req;
        req.destination_port = host_cast<PoliceService::port_type>(getPortNumber());
        req.gn.transport_type = geonet::TransportType::SHB;
        req.gn.traffic_class.tc_id(static_cast<unsigned>(dcc::Profile::DP1));
        req.gn.communication_profile = geonet::CommunicationProfile::ITS_G5;

        const std::string id = mVehicleController->getVehicleId();
        auto& vehicle_api = mVehicleController->getLiteAPI().vehicle();

        auto packet = new PoliceClearLane();
        packet->setEdgeName(vehicle_api.getRoadID(id).c_str());
        packet->setLaneIndex(vehicle_api.getLaneIndex(id));
        packet->setByteLength(40);
        request(req, packet);
    }
}

// is triggered when any signal is recieved
void PoliceServiceExtended::receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t signalID, omnetpp::cObject* signal, omnetpp::cObject*)
{
    // start the PoliceService after receiving Storyboard Signal
    if (signalID == storyboardSignal) {
        isTriggered = true;
    }
}
