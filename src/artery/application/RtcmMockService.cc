/*
* Artery V2X Simulation Framework
* Copyright 2019 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/application/RtcmMockMessage.h"
#include "artery/application/RtcmMockService.h"
#include <omnetpp/cmessage.h>
#include <omnetpp/cpacket.h>

namespace artery
{

Define_Module(RtcmMockService)

namespace {
    using namespace omnetpp;
    const simsignal_t rtcmSentSignal = cComponent::registerSignal("RtcmSent");
} // namespace

RtcmMockService::~RtcmMockService()
{
    cancelAndDelete(mTrigger);
}

void RtcmMockService::initialize()
{
    ItsG5Service::initialize();
    mTrigger = new omnetpp::cMessage("triggger RTCM mock message");
    mPositionProvider = &getFacilities().get_const<PositionProvider>();
    mHostId = getFacilities().get_const<Identity>().host->getId();

    namespace si = vanetza::units::si;
    mGnScf = par("gnStoreCarryForwarding");
    mGnRadius = par("gnRadius").doubleValue() * si::meter;
    mGnLifetime.encode(par("gnLifetime").doubleValue() * si::second);
    mDccProfile = par("dccProfile");
    mMessageLength = par("rtcmLength");

    mInterval = par("generationInterval");
    scheduleAt(omnetpp::simTime() + par("generationOffset"), mTrigger);
}

void RtcmMockService::handleMessage(omnetpp::cMessage* msg)
{
    if (msg == mTrigger) {
        generatePacket();
        scheduleAt(omnetpp::simTime() + mInterval, mTrigger);
    } else {
        ItsG5Service::handleMessage(msg);
    }
}

void RtcmMockService::generatePacket()
{
    using namespace vanetza;
    btp::DataRequestB req;
    req.destination_port = host_cast<RtcmMockService::port_type>(getPortNumber());
    req.gn.transport_type = geonet::TransportType::GBC;
    req.gn.traffic_class.tc_id(mDccProfile);
    req.gn.traffic_class.store_carry_forward(mGnScf);
    req.gn.communication_profile = geonet::CommunicationProfile::ITS_G5;

    geonet::Area destination;
    geonet::Circle circle;
    circle.r = mGnRadius;
    destination.shape = circle;
    destination.position.latitude = mPositionProvider->getGeodeticPosition().latitude;
    destination.position.longitude = mPositionProvider->getGeodeticPosition().longitude;
    req.gn.destination = destination;

    auto packet = new RtcmMockMessage();
    packet->setSourceStation(mHostId);
    packet->setSourcePosition(mPositionProvider->getCartesianPosition());
    packet->setByteLength(mMessageLength);
    emit(rtcmSentSignal, packet);
    request(req, packet);
}

} // namespace artery
