//
// Copyright (C) 2014 Raphael Riebl <raphael.riebl@thi.de>
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

#ifndef EDCAQUEUE_H_
#define EDCAQUEUE_H_

#include "AccessCategories.h"
#include <omnetpp/cpacket.h>
#include <omnetpp/crng.h>
#include <omnetpp/simtime.h>
#include <deque>
#include <boost/optional/optional.hpp>

class CarrierSensing;

class EdcaQueue
{
	public:
		enum BackoffReason {
			CHANNEL_BUSY,
			TXOP_FINISHED_SUCCESSFUL,
			TXOP_INITIAL_FAILURE,
			INTERNAL_COLLISION
		};

		struct Statistics {
				Statistics() : backoffTimes(0), backoffSlots(0), internalContentions(0) {}
				long backoffTimes;
				long backoffSlots;
				long internalContentions;
		};

		static const unsigned scDot11ShortRetryLimit = 7; // 802.11-2012, Annex C
		static const unsigned scDot11LongRetryLimit = 4; // 802.11-2012, Annex C
		static const unsigned scDot11RTSThreshold = 1000; // octets, see ES 202 663, A.1

		EdcaQueue();
		EdcaQueue(unsigned aifsn, unsigned cwmin, unsigned cwmax, omnetpp::cRNG*);
		virtual ~EdcaQueue();
		void setQueueSize(std::size_t size) { mQueueSize = size; }
		bool queuePacket(omnetpp::cPacket*, boost::optional<omnetpp::SimTime> idleDuration);
		omnetpp::cPacket* getReadyPacket();
		void doContention(omnetpp::SimTime idleDuration);
		void backoff(BackoffReason reason);
		void txFailure();
		void txSuccess();
		boost::optional<omnetpp::SimTime> getNextEventSlot(const CarrierSensing&);
		const Statistics& statistics() const { return mStatistics; }

	private:
		bool canTransmitImmediately(boost::optional<omnetpp::SimTime> idleDuration);
		void updateContentionWindow();

		typedef std::deque<omnetpp::cPacket*> PacketQueue;

		const unsigned mAifsn;
		const unsigned mCWmin;
		const unsigned mCWmax;
		PacketQueue mPackets;
		Statistics mStatistics;
		unsigned mContentionWindow;
		unsigned mBackoffSlots;
		unsigned mQSRC; // QoS short retry counter
		unsigned mQLRC; // QoS long retry counter
		std::size_t mQueueSize;
		omnetpp::cRNG* mRng;
};

template<enum edca::AccessCategory AC>
EdcaQueue createEdcaQueue(omnetpp::cRNG* rng)
{
	return EdcaQueue(edca::AccessCategoryTraits<AC>::AIFSN,
			edca::AccessCategoryTraits<AC>::CW_min,
			edca::AccessCategoryTraits<AC>::CW_max, rng);
}

#endif /* EDCAQUEUE_H_ */
