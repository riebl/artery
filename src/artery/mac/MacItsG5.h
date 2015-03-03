//
// MacItsG5 is based on Mac1609_4 included in Veins
//
// Original author:
// Copyright (C) 2012 David Eckhoff <eckhoff@cs.fau.de>
// Documentation for these modules is at http://veins.car2x.org/
//
// MacItsG5 author:
// Copyright (C) 2014 Raphael Riebl <raphael.riebl.inf@thi.de>
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

#ifndef MACITSG5_H_
#define MACITSG5_H_

#include "artery/mac/CarrierSensing.h"
#include "artery/mac/ChannelLoadMeasurements.h"
#include "artery/mac/Edca.h"
#include "veins/base/modules/BaseMacLayer.h"
#include "veins/base/utils/SimpleAddress.h"
#include "veins/modules/messages/Mac80211Pkt_m.h"
#include <omnetpp.h>

/**
 * @brief
 * Manages transmission queues with QoS priorities
 *
 * @author Raphael Riebl : adaption for ITS-G5
 * @author David Eckhoff : rewrote complete model
 * @author Christoph Sommer : features and bug fixes
 * @author Michele Segata : features and bug fixes
 * @author Stefan Joerer : features and bug fixes
 * @author Christopher Saloman: initial version
 *
 */
class MacItsG5 : public BaseMacLayer
{
	public:
		MacItsG5();
		virtual ~MacItsG5() {}

		struct Statistics {
				Statistics() :
					ReceivedPackets(0), ReceivedBroadcasts(0), SentPackets(0),
					TxRxLostPackets(0), SNIRLostPackets(0), DroppedPackets(0)
				{}

				long ReceivedPackets;
				long ReceivedBroadcasts;
				long SentPackets;
				long TxRxLostPackets; // a.k.a. collisions
				long SNIRLostPackets;
				long DroppedPackets;
				simtime_t TotalBusyTime;
		};

	protected:
		/** @brief Initialization of the module and some variables.*/
		virtual void initialize(int);

		/** @brief Delete all dynamically allocated objects of the module.*/
		virtual void finish();

		/** @brief Handle messages from lower layer.*/
		virtual void handleLowerMsg(cMessage*);

		/** @brief Handle messages from upper layer.*/
		virtual void handleUpperMsg(cMessage*);

		/** @brief Handle control messages from upper layer.*/
		virtual void handleUpperControl(cMessage* msg);

		/** @brief Handle self messages such as timers.*/
		virtual void handleSelfMsg(cMessage*);

		/** @brief Handle control messages from lower layer.*/
		virtual void handleLowerControl(cMessage* msg);

		/** @brief Self message to wake up at next MacEvent */
		cMessage* mNextMacEventMessage;

		/** @brief Tracking idle/busy states of medium */
		CarrierSensing mCarrierSensing;

		/** @brief EDCA subsystem */
		Edca mEdca;

		/** @brief Self message to generate a new channel load report */
		cMessage* mChannelLoadReport;

		/** @brief Duration between two channel load reports */
		simtime_t mChannelLoadReportInterval;

		/** @brief Buffer with channel load samples*/
		ChannelLoadMeasurements mChannelLoadMeasurements;

		/** @brief stats */
		Statistics mStatistics;

		/** @brief This MAC layers MAC address.*/
		LAddress::L2Type mMacAddress;

		/** @brief Length of MAC header in bits */
		unsigned mHeaderLength;

		/** @brief The power (in mW) to transmit with.*/
		double mTxPower;

		/** @brief the bit rate at which we transmit */
		uint64_t mBitrate;

		/** @brief Number of data bits per OFDM symbol */
		uint32_t mDataBitsPerSymbol;

	private:
		void attachSignal(Mac80211Pkt* mac, simtime_t startTime, double frequency);
		Signal* createSignal(simtime_t start, simtime_t length, double power, uint64_t bitrate, double frequency);
		void scheduleNextMacEvent();
		void writeRecord(const Statistics&, const Edca&);
		void setBitrate(uint64_t bitrate);

		simtime_t mIdleSince;
};


#endif /* MACITSG5_H_*/
