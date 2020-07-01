#include "artery/networking/GeoNetIndication.h"
#include "artery/networking/GeoNetRequest.h"
#include "artery/nic/RadioDriverProperties.h"
#include "artery/veins/VeinsMacFrame.h"
#include "artery/veins/VeinsRadioDriver.h"
#include "veins/base/utils/FindModule.h"
#include "veins/base/utils/SimpleAddress.h"
#include "veins/modules/mac/ieee80211p/Mac1609_4.h"
#include "veins/modules/utility/Consts80211p.h"

using namespace omnetpp;

namespace artery
{

Register_Class(VeinsRadioDriver)

namespace {

veins::LAddress::L2Type convert(const vanetza::MacAddress& mac)
{
    if (mac == vanetza::cBroadcastMacAddress) {
        return veins::LAddress::L2BROADCAST();
    } else {
        veins::LAddress::L2Type addr = 0;
        for (unsigned i = 0; i < mac.octets.size(); ++i) {
            addr <<= 8;
            addr |= mac.octets[i];
        }
        return addr;
    }
}

vanetza::MacAddress convert(veins::LAddress::L2Type addr)
{
    if (veins::LAddress::isL2Broadcast(addr)) {
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

int user_priority(vanetza::access::AccessCategory ac)
{
    using AC = vanetza::access::AccessCategory;
    int up = 0;
    switch (ac) {
        case AC::BK:
            up = 1;
            break;
        case AC::BE:
            up = 3;
            break;
        case AC::VI:
            up = 5;
            break;
        case AC::VO:
            up = 7;
            break;
    }
    return up;
}

const simsignal_t channelBusySignal = veins::Mac1609_4::sigChannelBusy;

} // namespace

void VeinsRadioDriver::initialize()
{
    RadioDriverBase::initialize();
    mHost = veins::FindModule<>::findHost(this);
    mHost->subscribe(channelBusySignal, this);

    mLowerLayerOut = gate("lowerLayerOut");
    mLowerLayerIn = gate("lowerLayerIn");

    mChannelLoadSampler.reset();
    mChannelLoadReport = new cMessage("report channel load");
    mChannelLoadReportInterval = par("channelLoadReportInterval");
    scheduleAt(simTime() + mChannelLoadReportInterval, mChannelLoadReport);

    auto properties = new RadioDriverProperties();
    // Mac1609_4 uses index of host as MAC address
    properties->LinkLayerAddress = convert(veins::LAddress::L2Type { mHost->getIndex() });
    // Mac1609_4 can only be fixed to CCH at the moment
    properties->ServingChannel = channel::CCH;
    indicateProperties(properties);
}

void VeinsRadioDriver::handleMessage(cMessage* msg)
{
    if (msg == mChannelLoadReport) {
        double channel_load = mChannelLoadSampler.cbr();
        emit(RadioDriverBase::ChannelLoadSignal, channel_load);
        scheduleAt(simTime() + mChannelLoadReportInterval, mChannelLoadReport);
    } else if (RadioDriverBase::isDataRequest(msg)) {
        handleDataRequest(msg);
    } else if (msg->getArrivalGate() == mLowerLayerIn) {
        handleDataIndication(msg);
    } else {
        throw cRuntimeError("unexpected message");
    }
}

void VeinsRadioDriver::handleDataIndication(cMessage* packet)
{
    auto frame = check_and_cast<VeinsMacFrame*>(packet);
    auto gn = frame->decapsulate();
    auto* indication = new GeoNetIndication();
    indication->source = convert(frame->getSenderAddress());
    indication->destination = convert(frame->getRecipientAddress());
    gn->setControlInfo(indication);
    delete frame;

    indicateData(gn);
}

void VeinsRadioDriver::handleDataRequest(cMessage* packet)
{
    auto request = check_and_cast<GeoNetRequest*>(packet->removeControlInfo());
    auto frame = new VeinsMacFrame();
    frame->encapsulate(check_and_cast<cPacket*>(packet));
    frame->setSenderAddress(convert(request->source_addr));
    frame->setRecipientAddress(convert(request->destination_addr));
    frame->setUserPriority(user_priority(request->access_category));
    frame->setChannelNumber(static_cast<int>(veins::Channel::cch));

    delete request;
    send(frame, mLowerLayerOut);
}

void VeinsRadioDriver::receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t signal, bool busy, omnetpp::cObject*)
{
    ASSERT(signal == channelBusySignal);
    mChannelLoadSampler.busy(busy);
}

} // namespace artery
