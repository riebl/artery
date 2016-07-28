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

#include "artery/mac/CarrierSensing.h"
#include "artery/mac/EdcaQueue.h"
#include "veins/modules/utility/Consts80211p.h"
#include <cassert>
#include <omnetpp/csimulation.h>
#include <omnetpp/distrib.h>

EdcaQueue::EdcaQueue() :
EdcaQueue(edca::AccessCategoryTraits<edca::AC_BK>::AIFSN,
		edca::AccessCategoryTraits<edca::AC_BK>::CW_min,
		edca::AccessCategoryTraits<edca::AC_BK>::CW_max, nullptr)
{
}

EdcaQueue::EdcaQueue(unsigned aifsn, unsigned cwmin, unsigned cwmax, omnetpp::cRNG* rng) :
		mAifsn(aifsn), mCWmin(cwmin), mCWmax(cwmax),
		mContentionWindow(cwmin), mBackoffSlots(0),
		mQSRC(0), mQLRC(0), mQueueSize(0),
		mRng(rng)
{
}

EdcaQueue::~EdcaQueue()
{
	for (omnetpp::cPacket* packet : mPackets) {
		delete packet;
	}
}

bool EdcaQueue::queuePacket(omnetpp::cPacket* packet, boost::optional<omnetpp::SimTime> idleDuration)
{
	if (mQueueSize == 0 || mPackets.size() < mQueueSize) {
		if (!canTransmitImmediately(idleDuration)) {
			backoff(CHANNEL_BUSY);
		}
		mPackets.push_back(packet);
		return true;
	} else {
		// queue full, do tail drop
		delete packet;
		return false;
	}
}

bool EdcaQueue::canTransmitImmediately(boost::optional<omnetpp::SimTime> idleDuration)
{
	if (mPackets.empty() && mBackoffSlots == 0 &&
			idleDuration && idleDuration.get() >= mAifsn * SLOTLENGTH_11P) {
		return true;
	} else {
		return false;
	}
}

omnetpp::cPacket* EdcaQueue::getReadyPacket()
{
	omnetpp::cPacket* packet = nullptr;
	if (!mPackets.empty() && mBackoffSlots == 0) {
		packet = mPackets.front();
	}

	return packet;
}

void EdcaQueue::doContention(omnetpp::SimTime idleDuration)
{
	idleDuration -= mAifsn * SLOTLENGTH_11P;
	if (idleDuration > 0.0) {
		unsigned passedSlots = idleDuration.raw() / SLOTLENGTH_11P.raw();
		mBackoffSlots -= std::min(mBackoffSlots, passedSlots);
	}
}

void EdcaQueue::backoff(BackoffReason reason)
{
	switch (reason) {
		case CHANNEL_BUSY:
			// no CW change
			break;
		case TXOP_FINISHED_SUCCESSFUL:
			mContentionWindow = mCWmin;
			break;
		case TXOP_INITIAL_FAILURE:
			updateContentionWindow();
			break;
		case INTERNAL_COLLISION:
			txFailure();
			updateContentionWindow();
			mStatistics.internalContentions++;
			break;
		default:
			throw omnetpp::cRuntimeError("Unknown backoff reason");
	}

	assert(mRng);
	mBackoffSlots = omnetpp::intuniform(mRng, 0, mContentionWindow);
	mStatistics.backoffTimes++;
	mStatistics.backoffSlots += mBackoffSlots;
}

void EdcaQueue::txFailure()
{
	omnetpp::cPacket* packet = mPackets.front();
	assert(packet != nullptr);
	if (packet->getByteLength() >= scDot11RTSThreshold) {
		++mQLRC;
	} else {
		++mQSRC;
	}
}

void EdcaQueue::txSuccess()
{
	omnetpp::cPacket* packet = mPackets.front();
	mPackets.pop_front();
	assert(packet != nullptr);

	backoff(TXOP_FINISHED_SUCCESSFUL);

	if (packet->getByteLength() >= scDot11RTSThreshold) {
		mQLRC = 0;
	} else {
		mQSRC = 0;
	}

	delete packet;
}

void EdcaQueue::updateContentionWindow()
{
	if (mQSRC >= scDot11ShortRetryLimit || mQLRC >= scDot11LongRetryLimit) {
		mContentionWindow = mCWmin;
	} else if (mContentionWindow < mCWmax) {
		mContentionWindow = (mContentionWindow + 1) * 2 - 1;
	} else {
		// remain unchanged
	}
}

boost::optional<omnetpp::SimTime> EdcaQueue::getNextEventSlot(const CarrierSensing& cs)
{
	boost::optional<omnetpp::SimTime> slot;
	if (cs.isIdle() && mPackets.size() > 0) {
		slot = cs.getStateSince() + (mAifsn + mBackoffSlots) * SLOTLENGTH_11P + SIFS_11P;
		if (slot <= omnetpp::simTime()) {
			slot = omnetpp::simTime();
		}
	}
	return slot;
}
