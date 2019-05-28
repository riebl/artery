#include "artery/inet/InetRadioDriver.h"
#include "artery/inet/VanetRx.h"
#include "artery/networking/GeoNetIndication.h"
#include "artery/networking/GeoNetRequest.h"
#include "artery/nic/RadioDriverProperties.h"
#include <inet/common/InitStages.h>
#include <inet/common/ModuleAccess.h>
#include <inet/linklayer/common/Ieee802Ctrl.h>
#include <inet/linklayer/ieee80211/mac/Ieee80211Mac.h>
#include <inet/physicallayer/ieee80211/packetlevel/Ieee80211Radio.h>

using namespace omnetpp;

namespace artery
{

Register_Class(InetRadioDriver)

namespace {

vanetza::MacAddress convert(const inet::MACAddress& mac)
{
	vanetza::MacAddress result;
	mac.getAddressBytes(result.octets.data());
	return result;
}

inet::MACAddress convert(const vanetza::MacAddress& mac)
{
	inet::MACAddress result;
	result.setAddressBytes(const_cast<uint8_t*>(mac.octets.data()));
	return result;
}

static const simsignal_t radioChannelChangedSignal = cComponent::registerSignal("radioChannelChanged");

} // namespace

int InetRadioDriver::numInitStages() const
{
	return inet::InitStages::NUM_INIT_STAGES;
}

void InetRadioDriver::initialize(int stage)
{
	if (stage == inet::INITSTAGE_LOCAL) {
		RadioDriverBase::initialize();
		cModule* host = inet::getContainingNode(this);
		mLinkLayer = inet::findModuleFromPar<inet::ieee80211::Ieee80211Mac>(par("macModule"), host);
		mLinkLayer->subscribe(VanetRx::ChannelLoadSignal, this);
		mRadio = inet::findModuleFromPar<inet::ieee80211::Ieee80211Radio>(par("radioModule"), host);
		mRadio->subscribe(radioChannelChangedSignal, this);
	} else if (stage == inet::InitStages::INITSTAGE_LINK_LAYER_2) {
		ASSERT(mChannelNumber > 0);
		auto properties = new RadioDriverProperties();
		properties->LinkLayerAddress = convert(mLinkLayer->getAddress());
		properties->ServingChannel = mChannelNumber;
		indicateProperties(properties);
	}
}

void InetRadioDriver::receiveSignal(cComponent* source, simsignal_t signal, double value, cObject*)
{
	if (signal == VanetRx::ChannelLoadSignal) {
		emit(RadioDriverBase::ChannelLoadSignal, value);
	}
}

void InetRadioDriver::receiveSignal(cComponent* source, simsignal_t signal, long value, cObject*)
{
	if (signal == radioChannelChangedSignal) {
		mChannelNumber = value;
	}
}

void InetRadioDriver::handleMessage(cMessage* msg)
{
	if (msg->getArrivalGate() == gate("lowerLayerIn")) {
		handleDataIndication(msg);
	} else {
		RadioDriverBase::handleMessage(msg);
	}
}

void InetRadioDriver::handleDataRequest(cMessage* packet)
{
	auto request = check_and_cast<GeoNetRequest*>(packet->removeControlInfo());
	auto ctrl = new inet::Ieee802Ctrl();
	ctrl->setDest(convert(request->destination_addr));
	ctrl->setSourceAddress(convert(request->source_addr));
	ctrl->setEtherType(request->ether_type.host());
	switch (request->access_category) {
		case vanetza::AccessCategory::VO:
			ctrl->setUserPriority(7);
			break;
		case vanetza::AccessCategory::VI:
			ctrl->setUserPriority(5);
			break;
		case vanetza::AccessCategory::BE:
			ctrl->setUserPriority(3);
			break;
		case vanetza::AccessCategory::BK:
			ctrl->setUserPriority(1);
			break;
		default:
			throw cRuntimeError("mapping to user priority (UP) unknown");
	}
	packet->setControlInfo(ctrl);
	delete request;

	send(packet, "lowerLayerOut");
}

void InetRadioDriver::handleDataIndication(cMessage* packet)
{
	auto* info = check_and_cast<inet::Ieee802Ctrl*>(packet->removeControlInfo());
	auto* indication = new GeoNetIndication();
	indication->source = convert(info->getSrc());
	indication->destination = convert(info->getDest());
	packet->setControlInfo(indication);
	delete info;

	indicateData(packet);
}

} // namespace artery
