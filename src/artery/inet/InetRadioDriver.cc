#include "artery/inet/InetRadioDriver.h"
#include "artery/inet/VanetRx.h"
#include "artery/networking/GeoNetIndication.h"
#include "artery/networking/GeoNetRequest.h"
#include "artery/nic/RadioDriverProperties.h"
#include <inet/common/InitStages.h>
#include <inet/common/ModuleAccess.h>
#include <inet/common/Protocol.h>
#include <inet/common/ProtocolGroup.h>
#include <inet/common/ProtocolTag_m.h>
#include <inet/common/packet/chunk/cPacketChunk.h>
#include <inet/linklayer/common/MacAddressTag_m.h>
#include <inet/linklayer/common/UserPriorityTag_m.h>
#include <inet/linklayer/ieee80211/mac/Ieee80211Mac.h>
#include <mutex>

using namespace omnetpp;

namespace artery
{

Register_Class(InetRadioDriver)

namespace {

vanetza::MacAddress convert(const inet::MacAddress& mac)
{
	vanetza::MacAddress result;
	mac.getAddressBytes(result.octets.data());
	return result;
}

inet::MacAddress convert(const vanetza::MacAddress& mac)
{
	inet::MacAddress result;
	result.setAddressBytes(const_cast<uint8_t*>(mac.octets.data()));
	return result;
}

std::once_flag register_protocol_flag;
const inet::Protocol geonet { "GeoNet", "ETSI ITS-G5 GeoNetworking", inet::Protocol::NetworkLayer };

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

		// we were allowed to call addProtocol each time but call_once makes more sense to me
		std::call_once(register_protocol_flag, []() {
			inet::ProtocolGroup::ethertype.addProtocol(0x8947, &geonet);
		});
	} else if (stage == inet::InitStages::INITSTAGE_LINK_LAYER_2) {
		auto properties = new RadioDriverProperties();
		properties->LinkLayerAddress = convert(mLinkLayer->getAddress());
		indicateProperties(properties);
	}
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
		handleDataIndication(msg);
	} else {
		RadioDriverBase::handleMessage(msg);
	}
}

void InetRadioDriver::handleDataRequest(cMessage* msg)
{
	auto request = check_and_cast<GeoNetRequest*>(msg->removeControlInfo());
	auto packet = new inet::Packet("INET GeoNet", inet::makeShared<inet::cPacketChunk>(check_and_cast<cPacket*>(msg)));

	auto addr_tag = packet->addTag<inet::MacAddressReq>();
	addr_tag->setDestAddress(convert(request->destination_addr));
	addr_tag->setSrcAddress(convert(request->source_addr));

	auto proto_tag = packet->addTagIfAbsent<inet::PacketProtocolTag>();
	proto_tag->setProtocol(&geonet);
	assert(request->ether_type.host() == inet::ProtocolGroup::ethertype.findProtocolNumber(&geonet));

	auto up_tag = packet->addTag<inet::UserPriorityReq>();
	switch (request->access_category) {
		case vanetza::AccessCategory::VO:
			up_tag->setUserPriority(7);
			break;
		case vanetza::AccessCategory::VI:
			up_tag->setUserPriority(5);
			break;
		case vanetza::AccessCategory::BE:
			up_tag->setUserPriority(3);
			break;
		case vanetza::AccessCategory::BK:
			up_tag->setUserPriority(1);
			break;
		default:
			error("mapping to user priority (UP) unknown");
	}
	delete request;

	send(packet, "lowerLayerOut");
}

void InetRadioDriver::handleDataIndication(cMessage* msg)
{
	auto packet = check_and_cast<inet::Packet*>(msg);
	auto chunk = packet->peekData<inet::cPacketChunk>();
	auto gn_packet = chunk->getPacket()->dup();

	auto addr_tag = packet->getTag<inet::MacAddressInd>();
	auto* indication = new GeoNetIndication();
	indication->source = convert(addr_tag->getSrcAddress());
	indication->destination = convert(addr_tag->getDestAddress());
	gn_packet->setControlInfo(indication);
	delete msg;

	indicateData(gn_packet);
}

} // namespace artery
