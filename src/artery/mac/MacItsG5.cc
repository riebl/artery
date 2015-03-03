//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "artery/mac/MacItsG5.h"
#include "artery/mac/MacToGeoNetControlInfo.h"
#include "artery/messages/ChannelLoadReport_m.h"
#include "artery/netw/GeoNetToMacControlInfo.h"
#include "veins/base/phyLayer/MacToPhyControlInfo.h"
#include "veins/base/phyLayer/MacToPhyInterface.h"
#include "veins/modules/phy/Decider80211p.h"
#include <vanetza/dcc/channel_load.hpp>

Define_Module(MacItsG5);

MacItsG5::MacItsG5() : mEdca(mCarrierSensing)
{
}

void MacItsG5::initialize(int stage)
{
	BaseMacLayer::initialize(stage);
	if (0 == stage) {
		// required to circumvent double precision issues with constants from CONST80211p.h
		assert(simTime().getScaleExp() == -12);

		mHeaderLength = par("headerLength");
		mTxPower = par("txPower");
		setBitrate(par("bitrate"));

		mMacAddress = 0; // set by GeoNet control info later on
		mNextMacEventMessage = new cMessage("next MAC event");
		mChannelLoadReport = new cMessage("report channel load");
		mChannelLoadReportInterval = simtime_t(100, SIMTIME_MS);
		mChannelLoadMeasurements.reset();
		scheduleAt(simTime() + mChannelLoadReportInterval, mChannelLoadReport);

		mStatistics = Statistics {};
		mCarrierSensing.setState(CarrierSensing::IDLE);
	}
}

void MacItsG5::finish()
{
	cancelAndDelete(mNextMacEventMessage);
	cancelAndDelete(mChannelLoadReport);
	writeRecord(mStatistics, mEdca);
}

void MacItsG5::handleSelfMsg(cMessage* msg)
{
	if (msg == mNextMacEventMessage) {
		auto idleDuration = mCarrierSensing.getIdleDuration();
		if (!idleDuration) {
			opp_error("MacItsG5 should receive self message only during idle medium");
		}
		mCarrierSensing.setState(CarrierSensing::BUSY_SELF);
		mEdca.doContention(idleDuration.get());
		cPacket* net = mEdca.initiateTransmission();
		assert(net != nullptr);

		//send the packet
		Mac80211Pkt* mac = new Mac80211Pkt(net->getName(), net->getKind());
		mac->setBitLength(mHeaderLength);
		GeoNetToMacControlInfo* netInfo = dynamic_cast<GeoNetToMacControlInfo*>(net->getControlInfo());
		assert(netInfo);
		mac->setDestAddr(netInfo->destination_addr);
		mac->setSrcAddr(netInfo->source_addr);
		mac->encapsulate(net->dup());

		phy->setRadioState(Veins::Radio::TX);
		const double cFreqG5CC = 5.90e9;
		attachSignal(mac, simTime() + RADIODELAY_11P, cFreqG5CC);
		MacToPhyControlInfo* phyInfo = dynamic_cast<MacToPhyControlInfo*>(mac->getControlInfo());
		assert(phyInfo);
		sendDelayed(mac, RADIODELAY_11P, lowerLayerOut);
		mStatistics.SentPackets++;
	} else if (msg == mChannelLoadReport) {
		auto* report = new ChannelLoadReport();
		report->setChannelLoad(mChannelLoadMeasurements.channel_load());
		sendControlUp(report);
		scheduleAt(simTime() + mChannelLoadReportInterval, mChannelLoadReport);
	} else {
		opp_error("Unknown MacItsG5 self-message");
	}
}

void MacItsG5::handleUpperControl(cMessage* msg)
{
	opp_error("No implementation of MacItsG5 upper control messages");
}

void MacItsG5::handleUpperMsg(cMessage* msg)
{
	cPacket* packet = dynamic_cast<cPacket*>(msg);
	if (packet == nullptr) {
		opp_error("MacItsG5 requires packet from upper layer");
	}

	GeoNetToMacControlInfo* macCtrlInfo = dynamic_cast<GeoNetToMacControlInfo*>(packet->getControlInfo());
	if (macCtrlInfo == nullptr) {
		opp_error("MacItsG5 requires message with GeoNetToMacControlInfo from upper layer");
	} else {
		// actually GeoNet layer determines MAC address
		mMacAddress = macCtrlInfo->source_addr;
	}

	if (mEdca.queuePacket(macCtrlInfo->access_category, packet)) {
		scheduleNextMacEvent();
	} else {
		mStatistics.DroppedPackets++;
	}
}

void MacItsG5::handleLowerControl(cMessage* msg)
{
	if (msg->getKind() == MacToPhyInterface::TX_OVER) {
		phy->setRadioState(Veins::Radio::RX);
		mEdca.txSuccess();
	} else if (msg->getKind() == Mac80211pToPhy11pInterface::CHANNEL_BUSY) {
		mChannelLoadMeasurements.busy();
		auto idleDuration = mCarrierSensing.getIdleDuration();
		mCarrierSensing.setState(CarrierSensing::BUSY_PHYSICAL);
		if (idleDuration) {
			mEdca.doContention(idleDuration.get());
		}
		cancelEvent(mNextMacEventMessage);
	} else if (msg->getKind() == Mac80211pToPhy11pInterface::CHANNEL_IDLE) {
		mChannelLoadMeasurements.idle();
		mCarrierSensing.setState(CarrierSensing::IDLE);
		scheduleNextMacEvent();
	} else if (msg->getKind() == Decider80211p::BITERROR) {
		mStatistics.SNIRLostPackets++;
	} else if (msg->getKind() == Decider80211p::RECWHILESEND) {
		mStatistics.TxRxLostPackets++;
	} else if (msg->getKind() == MacToPhyInterface::RADIO_SWITCHING_OVER) {
	} else if (msg->getKind() == BaseDecider::PACKET_DROPPED) {
		mEdca.txFailure();
		phy->setRadioState(Veins::Radio::RX);
		mStatistics.DroppedPackets++;
	} else {
		opp_error("Unknown MacItsG5 lower control message");
	}

	delete msg;
}

void MacItsG5::handleLowerMsg(cMessage* msg)
{
	Mac80211Pkt* macPacket = dynamic_cast<Mac80211Pkt*>(msg);
	assert(macPacket);

	cPacket* packetUp = macPacket->decapsulate();
	const LAddress::L2Type destMac = macPacket->getDestAddr();
	auto* ctrlInfo = new MacToGeoNetControlInfo();
	ctrlInfo->source_addr = macPacket->getSrcAddr();
	ctrlInfo->destination_addr = macPacket->getDestAddr();
	packetUp->setControlInfo(ctrlInfo);

	if (destMac == mMacAddress) {
		mStatistics.ReceivedPackets++;
		sendUp(packetUp);
	} else if (destMac == LAddress::L2BROADCAST) {
		mStatistics.ReceivedBroadcasts++;
		sendUp(packetUp);
	} else {
		delete packetUp;
	}

	delete macPacket;
}

void MacItsG5::attachSignal(Mac80211Pkt* mac, simtime_t startTime, double frequency)
{
	int macPktlen = mac->getBitLength();
	simtime_t duration =
			PHY_HDR_PREAMBLE_DURATION +
			PHY_HDR_PLCPSIGNAL_DURATION +
			T_SYM_80211P * std::ceil((macPktlen + 22) / mDataBitsPerSymbol);

	Signal* s = createSignal(startTime, duration, mTxPower, mBitrate, frequency);
	MacToPhyControlInfo* cinfo = new MacToPhyControlInfo(s);

	mac->setControlInfo(cinfo);
}

Signal* MacItsG5::createSignal(simtime_t start, simtime_t length, double power, uint64_t bitrate, double frequency) {
	simtime_t end = start + length;
	//create signal with start at current simtime and passed length
	Signal* s = new Signal(start, length);

	//create and set tx power mapping
	ConstMapping* txPowerMapping = createSingleFrequencyMapping(start, end, frequency, 5.0e6, power);
	s->setTransmissionPower(txPowerMapping);

	Mapping* bitrateMapping = MappingUtils::createMapping(DimensionSet::timeDomain, Mapping::STEPS);

	Argument pos(start);
	bitrateMapping->setValue(pos, bitrate);

	pos.setTime(phyHeaderLength / bitrate);
	bitrateMapping->setValue(pos, bitrate);

	s->setBitrate(bitrateMapping);

	return s;
}

void MacItsG5::writeRecord(const Statistics& stats, const Edca& edca)
{
	recordScalar("ReceivedUnicastPackets", stats.ReceivedPackets);
	recordScalar("ReceivedBroadcasts", stats.ReceivedBroadcasts);
	recordScalar("SentPackets", stats.SentPackets);
	recordScalar("SNIRLostPackets", stats.SNIRLostPackets);
	recordScalar("RxTxLostPackets", stats.TxRxLostPackets);
	recordScalar("TotalLostPackets", stats.SNIRLostPackets + stats.TxRxLostPackets);
	recordScalar("DroppedPacketsInMac",stats.DroppedPackets);
	recordScalar("TimesIntoBackoff", edca.getBackoffTimes());
	recordScalar("SlotsBackoff", edca.getBackoffSlots());
	recordScalar("InternalContentions", edca.getInternalContentions());
	recordScalar("TotalBusyTime", stats.TotalBusyTime.dbl());
}

void MacItsG5::scheduleNextMacEvent()
{
	boost::optional<simtime_t> time = mEdca.getNextEventSlot();
	if (time) {
		if (time.get() > simTime()) {
			cancelEvent(mNextMacEventMessage);
			scheduleAt(time.get(), mNextMacEventMessage);
			EV << "mac event; now: " << simTime() << " scheduled: " << time.get() << endl;
		} else if (time.get() == simTime()) {
			// This can occur for immediate transmissions
			handleSelfMsg(mNextMacEventMessage);
		} else {
			opp_error("Can't schedule events in the past");
		}
	}
}

void MacItsG5::setBitrate(uint64_t bitrate)
{
	for (unsigned i = 0; i < sizeof(BITRATES_80211P); ++i) {
		if (bitrate == BITRATES_80211P[i]) {
			mBitrate = bitrate;
			mDataBitsPerSymbol = N_DBPS_80211P[i];
			return;
		}
	}
	opp_error("Invalid 802.11p bitrate");
}
