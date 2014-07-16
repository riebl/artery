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

#include "Edca.h"
#include <boost/optional/optional.hpp>

using namespace edca;

Edca::Edca(const CarrierSensing& cs) : mCarrierSensing(cs)
{
	// create queues for all ACs, highest priority first
	mQueues.insert(std::make_pair(AC_VO, createEdcaQueue<AC_VO>()));
	mQueues.insert(std::make_pair(AC_VI, createEdcaQueue<AC_VI>()));
	mQueues.insert(std::make_pair(AC_BE, createEdcaQueue<AC_BE>()));
	mQueues.insert(std::make_pair(AC_BK, createEdcaQueue<AC_BK>()));
}

Edca::~Edca()
{
}

void Edca::setQueueSize(std::size_t size)
{
	for (auto& iter : mQueues) {
		iter.second.setQueueSize(size);
	}
}

bool Edca::queuePacket(enum AccessCategory ac, cPacket* packet)
{
	return mQueues[ac].queuePacket(packet, mCarrierSensing.getIdleDuration());
}

cPacket* Edca::initiateTransmission()
{
	cPacket* readyPacket = nullptr;

	for (auto& iter : mQueues) {
		cPacket* queueReadyPacket = iter.second.getReadyPacket();
		if (queueReadyPacket == nullptr) continue;
		if (readyPacket == nullptr) {
			// highest priority message found
			readyPacket = queueReadyPacket;
			mLastAC = iter.first;
		} else {
			// internal contention, backoff lower priority queue
			iter.second.backoff(EdcaQueue::INTERNAL_COLLISION);
		}
	}

	return readyPacket;
}

void Edca::doContention(simtime_t idleDuration)
{
	for (auto& iter : mQueues) {
		iter.second.doContention(idleDuration);
		if (mCarrierSensing.getState() == CarrierSensing::BUSY_PHYSICAL && iter.second.getReadyPacket() != nullptr) {
			// Queue would be ready to send now, but medium is busy: invoke backoff
			iter.second.backoff(EdcaQueue::TXOP_INITIAL_FAILURE);
		}
	}
}

void Edca::txSuccess()
{
	mQueues.at(mLastAC).txSuccess();
}

void Edca::txFailure()
{
	mQueues.at(mLastAC).txFailure();
}

boost::optional<simtime_t> Edca::getNextEventSlot()
{
	boost::optional<simtime_t> slot;

	for (auto& iter : mQueues) {
		boost::optional<simtime_t> possible_slot = iter.second.getNextEventSlot(mCarrierSensing);
		if (!slot && possible_slot) {
			slot = possible_slot;
		} else if (possible_slot) {
			slot = std::min(slot, possible_slot);
		}
	}

	return slot;
}

long Edca::getBackoffTimes() const
{
	long times = 0;
	for (const auto& iter : mQueues) {
		times += iter.second.statistics().backoffTimes;
	}
	return times;
}

long Edca::getBackoffSlots() const
{
	long slots = 0;
	for (const auto& iter : mQueues) {
		slots += iter.second.statistics().backoffSlots;
	}
	return slots;
}

long Edca::getInternalContentions() const
{
	long contentions = 0;
	for (const auto& iter : mQueues) {
		contentions += iter.second.statistics().internalContentions;
	}
	return contentions;
}
