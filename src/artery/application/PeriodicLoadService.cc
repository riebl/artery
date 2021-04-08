/*
* Artery V2X Simulation Framework
* Copyright 2020 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "PeriodicLoadService.h"
#include <omnetpp/cpacket.h>
#include <vanetza/btp/data_request.hpp>
#include <vanetza/dcc/profile.hpp>
#include <vanetza/geonet/interface.hpp>

using namespace omnetpp;
using namespace vanetza;

namespace artery
{

Define_Module(PeriodicLoadService)

PeriodicLoadService::PeriodicLoadService()
{
}

PeriodicLoadService::~PeriodicLoadService()
{
    cancelAndDelete(mTrigger);
}

void PeriodicLoadService::indicate(const btp::DataIndication& ind, cPacket* packet, const NetworkInterface& net)
{
    Enter_Method("indicate");
    delete(packet);
}

void PeriodicLoadService::initialize()
{
    ItsG5Service::initialize();
    mAppId = par("aid");
    mTrigger = new cMessage("PeriodicLoadService generate message");
    mTrigger->setSchedulingPriority(1); // this trigger is then after radio setup
    if (!par("waitForFirstTrigger")) {
        scheduleAt(simTime(), mTrigger);
    }
}

void PeriodicLoadService::handleMessage(cMessage* msg)
{
    if (msg == mTrigger) {
        generateTransmissionRequest();
        scheduleTransmission();
    }
}

void PeriodicLoadService::trigger()
{
    Enter_Method("trigger");
    if (!mTrigger->isScheduled()) {
        generateTransmissionRequest();
        scheduleTransmission();
    }
}

void PeriodicLoadService::scheduleTransmission()
{
    scheduleAt(simTime() + par("generationInterval"), mTrigger);
}

void PeriodicLoadService::generateTransmissionRequest()
{
    auto& mco = getFacilities().get_const<MultiChannelPolicy>();
    auto& networks = getFacilities().get_const<NetworkInterfaceTable>();

    for (auto channel : mco.allChannels(mAppId)) {
        auto network = networks.select(channel);
        if (network) {
            btp::DataRequestB req;
            // use same port number as configured for listening on this channel
            req.destination_port = host_cast(getPortNumber(channel));
            req.gn.transport_type = geonet::TransportType::SHB;
            req.gn.traffic_class.tc_id(static_cast<unsigned>(dcc::Profile::DP2));
            req.gn.communication_profile = geonet::CommunicationProfile::ITS_G5;
            req.gn.its_aid = mAppId;

            cPacket* packet = new cPacket("PeriodicLoadService packet");
            packet->setByteLength(par("payloadLength"));

            // send packet on specific network interface
            request(req, packet, network.get());
        } else {
            EV_ERROR << "No network interface available for channel " << channel << "\n";
        }
    }
}

} // namespace artery
