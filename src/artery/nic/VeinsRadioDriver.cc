#include "artery/nic/VeinsRadioDriver.h"
#include "artery/netw/GeoNetIndication.h"
#include "artery/netw/GeoNetRequest.h"
#include "artery/netw/GeoNetToMacControlInfo.h"
#include "artery/mac/MacToGeoNetControlInfo.h"
#include "artery/mac/AccessCategoriesVanetza.h"
#include "artery/messages/ChannelLoadReport_m.h"

Register_Class(VeinsRadioDriver)

namespace {

LAddress::L2Type convert(const vanetza::MacAddress& mac)
{
    if (mac == vanetza::cBroadcastMacAddress) {
        return LAddress::L2BROADCAST();
    } else {
        LAddress::L2Type addr = 0;
        for (unsigned i = 0; i < mac.octets.size(); ++i) {
            addr <<= 8;
            addr |= mac.octets[i];
        }
        return addr;
    }
}

vanetza::MacAddress convert(LAddress::L2Type addr)
{
    if (LAddress::isL2Broadcast(addr)) {
        return vanetza::cBroadcastMacAddress;
    } else {
        vanetza::MacAddress mac;
        for (unsigned i = mac.octets.size(); i > 0; --i) {
            mac.octets[i - 1] = addr & 0xff;
            addr >>= 8;
        }
        return mac;
    }
}

} // namespace

vanetza::MacAddress VeinsRadioDriver::getMacAddress()
{
    return vanetza::create_mac_address(this->getId());
}

void VeinsRadioDriver::initialize()
{
    RadioDriverBase::initialize();
    mLowerLayerOut = gate("lowerLayerOut");
    mLowerLayerIn = gate("lowerLayerIn");
    mLowerControlIn = gate("lowerControlIn");
}

void VeinsRadioDriver::handleMessage(cMessage* msg)
{
    if (RadioDriverBase::isMiddlewareRequest(msg)) {
        handleUpperMessage(msg);
    } else if (msg->getArrivalGate() == mLowerLayerIn) {
        handleLowerMessage(msg);
    } else if (msg->getArrivalGate() == mLowerControlIn) {
        handleLowerControl(msg);
    } else {
        throw cRuntimeError("unexpected message");
    }
}

void VeinsRadioDriver::handleLowerMessage(cMessage* packet)
{
    auto info = check_and_cast<MacToGeoNetControlInfo*>(packet->removeControlInfo());
    auto* indication = new GeoNetIndication();
    indication->source = convert(info->source_addr);
    indication->destination = convert(info->destination_addr);
    packet->setControlInfo(indication);
    delete info;

    indicatePacket(packet);
}

void VeinsRadioDriver::handleLowerControl(cMessage* msg)
{
    auto channelLoadReport = check_and_cast<ChannelLoadReport*>(msg);
    double channelLoad = channelLoadReport->getChannelLoad().fraction();
    emit(RadioDriverBase::ChannelLoadSignal, channelLoad);
    delete msg;
}

void VeinsRadioDriver::handleUpperMessage(cMessage* packet)
{
    auto request = check_and_cast<GeoNetRequest*>(packet->removeControlInfo());
    auto ctrl = new GeoNetToMacControlInfo();
    ctrl->access_category = edca::map(request->access_category);
    ctrl->destination_addr = convert(request->destination_addr);
    ctrl->source_addr = convert(request->source_addr);
    packet->setControlInfo(ctrl);

    send(packet, mLowerLayerOut);
}
