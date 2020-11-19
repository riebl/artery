/*
* Artery V2X Simulation Framework
* Copyright 2019 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/application/InfrastructureMockService.h"
#include "artery/application/InfrastructureMockMessage.h"
#include <boost/format.hpp>
#include <omnetpp/cmessage.h>
#include <omnetpp/cpacket.h>
#include <omnetpp/cwatch.h>

namespace artery
{

Define_Module(InfrastructureMockService)

namespace {
    using namespace omnetpp;
    const simsignal_t immSentSignal = cComponent::registerSignal("ImmSent");
} // namespace

InfrastructureMockService::~InfrastructureMockService()
{
    cancelAndDelete(mTrigger);
}

void InfrastructureMockService::initialize()
{
    ItsG5Service::initialize();
    mTrigger = new omnetpp::cMessage("triggger infrastructure mock message");
    mPositionProvider = &getFacilities().get_const<PositionProvider>();
    mHostId = getFacilities().get_const<Identity>().host->getId();

    mPacketName = (boost::format("%1% mock-up packet") % this->getName()).str();
    mMessageLength = par("messageLength");
    if (mMessageLength < par("messageLengthMin").intValue()) {
        mMessageLength = par("messageLengthMin");
    } else if (mMessageLength > par("messageLengthMax").intValue()) {
        mMessageLength = par("messageLengthMax");
    }
    omnetpp::createWatch("messageLength", mMessageLength);

    mDisseminationRadius = par("disseminationRadius").doubleValue() * boost::units::si::meter;
    mPacketPriority = par("packetPriority");

    mInterval = par("generationInterval");
    scheduleAt(omnetpp::simTime() + par("generationOffset"), mTrigger);
}

void InfrastructureMockService::handleMessage(omnetpp::cMessage* msg)
{
    if (msg == mTrigger) {
        generatePacket();
        scheduleAt(omnetpp::simTime() + mInterval, mTrigger);
    } else {
        ItsG5Service::handleMessage(msg);
    }
}

void InfrastructureMockService::generatePacket()
{
    using namespace vanetza;
    btp::DataRequestB req;
    req.destination_port = host_cast<InfrastructureMockService::port_type>(getPortNumber());
    req.gn.transport_type = geonet::TransportType::GBC;
    req.gn.traffic_class.tc_id(mPacketPriority);
    req.gn.communication_profile = geonet::CommunicationProfile::ITS_G5;

    geonet::Area destination;
    geonet::Circle circle;
    circle.r = mDisseminationRadius;
    destination.shape = circle;
    destination.position.latitude = mPositionProvider->getGeodeticPosition().latitude;
    destination.position.longitude = mPositionProvider->getGeodeticPosition().longitude;
    req.gn.destination = destination;

    auto packet = new InfrastructureMockMessage();
    packet->setSourceStation(mHostId);
    packet->setSequenceNumber(mSequenceNumber++);
    packet->setByteLength(mMessageLength);
    emit(immSentSignal, packet);
    request(req, packet);
}

} // namespace artery
