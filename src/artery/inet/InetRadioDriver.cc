#include "artery/inet/InetRadioDriver.h"
#include "artery/inet/VanetRx.h"
#include "artery/networking/GeoNetIndication.h"
#include "artery/networking/GeoNetRequest.h"
#include <inet/common/ModuleAccess.h>
#include <inet/linklayer/common/Ieee802Ctrl.h>
#include <inet/linklayer/ieee80211/mac/Ieee80211Mac.h>

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

} // namespace

vanetza::MacAddress InetRadioDriver::getMacAddress()
{
	return convert(mLinkLayer->getAddress());
}

void InetRadioDriver::initialize()
{
	RadioDriverBase::initialize();
	cModule* host = inet::getContainingNode(this);
	mLinkLayer = inet::findModuleFromPar<inet::ieee80211::Ieee80211Mac>(par("macModule"), host);
	mLinkLayer->subscribe(VanetRx::ChannelLoadSignal, this);
}

void InetRadioDriver::receiveSignal(cComponent* source, simsignal_t signal, double value, cObject*)
{
	if (signal == VanetRx::ChannelLoadSignal) {
		emit(RadioDriverBase::ChannelLoadSignal, value);
	}
}

void InetRadioDriver::handleMessage(cMessage* msg)
{
	if (msg->getArrivalGate() == gate("lowerLayerIn")) {
		handleLowerMessage(msg);
	} else {
		RadioDriverBase::handleMessage(msg);
	}
}

void InetRadioDriver::handleUpperMessage(cMessage* packet)
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

void InetRadioDriver::handleLowerMessage(cMessage* packet)
{
	auto* info = check_and_cast<inet::Ieee802Ctrl*>(packet->removeControlInfo());
	auto* indication = new GeoNetIndication();
	indication->source = convert(info->getSrc());
	indication->destination = convert(info->getDest());
	packet->setControlInfo(indication);
	delete info;

	indicatePacket(packet);
}

} // namespace artery
