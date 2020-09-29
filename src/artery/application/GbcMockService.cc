/*
* Artery V2X Simulation Framework
* Copyright 2020 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/application/GbcMockMessage.h"
#include "artery/application/GbcMockService.h"
#include "artery/application/StoryboardSignal.h"
#include <inet/common/geometry/common/Coord.h>
#include <omnetpp/cmessage.h>
#include <omnetpp/cpacket.h>

namespace artery
{

Define_Module(GbcMockService)

using namespace omnetpp;

namespace
{
static const simsignal_t scSignalReceived = cComponent::registerSignal("GbcMockReceived");
static const simsignal_t scSignalSent = cComponent::registerSignal("GbcMockSent");
static const simsignal_t scSignalStoryboard = cComponent::registerSignal("StoryboardSignal");
} // namespace

GbcMockService::~GbcMockService()
{
    cancelAndDelete(mTrigger);
}

void GbcMockService::initialize()
{
    ItsG5Service::initialize();
    mTrigger = new cMessage("triggger GBC mock message");
    mPositionProvider = &getFacilities().get_const<PositionProvider>();
    mHostId = getFacilities().get_const<Identity>().host->getId();

    namespace chrono = std::chrono;
    namespace si = vanetza::units::si;
    mGnScf = par("gnStoreCarryForwarding");
    mGnRadius = par("gnRadius").doubleValue() * si::meter;
    mGnLifetime.encode(par("gnLifetime").doubleValue() * si::second);
    mGnRepetitionInterval = par("gnRepetitionInterval").doubleValue() * si::second;
    mGnRepetitionMaximum = par("gnRepetitionMaximum").doubleValue() * si::second;
    mDccProfile = par("dccProfile");

    mPacketLimit = par("packetLimit");
    mMessageLength = par("payloadLength");
    if (mPacketLimit != 0) {
        scheduleAt(simTime() + par("generationOffset"), mTrigger);
    }
    WATCH(mPacketCounter);
    subscribe(scSignalStoryboard);
}

void GbcMockService::handleMessage(cMessage* msg)
{
    if (msg == mTrigger) {
        ++mPacketCounter;
        generatePacket();
        if (mPacketLimit < 0 || mPacketLimit > mPacketCounter) {
            scheduleAt(simTime() + par("generationInterval"), mTrigger);
        }
    } else {
        ItsG5Service::handleMessage(msg);
    }
}

void GbcMockService::generatePacket()
{
    using namespace vanetza;
    btp::DataRequestB req;
    req.destination_port = host_cast<GbcMockService::port_type>(getPortNumber());
    req.gn.transport_type = geonet::TransportType::GBC;
    req.gn.traffic_class.tc_id(mDccProfile);
    req.gn.traffic_class.store_carry_forward(mGnScf);
    req.gn.communication_profile = geonet::CommunicationProfile::ITS_G5;
    req.gn.maximum_lifetime = mGnLifetime;
    if (mGnRepetitionInterval > 0.0 * units::si::second && mGnRepetitionMaximum > 0.0 * units::si::second) {
        req.gn.repetition = geonet::DataRequest::Repetition {};
        req.gn.repetition->interval = mGnRepetitionInterval;
        req.gn.repetition->maximum = mGnRepetitionMaximum;
    }

    geonet::Area destination;
    geonet::Circle circle;
    circle.r = mGnRadius;
    destination.shape = circle;
    if (par("disseminateAroundStation")) {
        destination.position.latitude = mPositionProvider->getGeodeticPosition().latitude;
        destination.position.longitude = mPositionProvider->getGeodeticPosition().longitude;
    } else {
        destination.position.latitude = par("gnDestinationLatitude").doubleValue() * units::degree;
        destination.position.longitude = par("gnDestinationLongitude").doubleValue() * units::degree;
    }
    req.gn.destination = destination;

    auto packet = new GbcMockMessage();
    packet->setSourceStation(mHostId);
    packet->setSourcePosition(mPositionProvider->getCartesianPosition());
    packet->setByteLength(mMessageLength);
    emit(scSignalSent, packet);
    request(req, packet);
}

void GbcMockService::indicate(const vanetza::btp::DataIndication&, cPacket* packet)
{
    const auto cartesian = mPositionProvider->getCartesianPosition();
    inet::Coord position { cartesian.x.value(), cartesian.y.value() };
    emit(scSignalReceived, omnetpp::check_and_cast<GbcMockMessage*>(packet), &position);
    delete packet;
}

void GbcMockService::receiveSignal(cComponent*, simsignal_t sig, cObject* obj, cObject*)
{
    if (sig == scSignalStoryboard) {
        auto sigobj = check_and_cast<artery::StoryboardSignal*>(obj);
        if (sigobj->getCause() == par("generationSignal").stdstringValue()) {
            ++mPacketCounter;
            generatePacket();
        }
    }
}

} // namespace artery
