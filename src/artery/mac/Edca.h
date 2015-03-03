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

#ifndef EDCA_H_
#define EDCA_H_

#include "artery/mac/AccessCategories.h"
#include "artery/mac/CarrierSensing.h"
#include "artery/mac/EdcaQueue.h"
#include "veins/modules/utility/Consts80211p.h"
#include <cmessage.h>
#include <simtime_t.h>
#include <deque>
#include <map>
#include <boost/optional/optional.hpp>

class Edca
{
	public:
		Edca(const CarrierSensing&);
		virtual ~Edca();
		bool queuePacket(enum edca::AccessCategory, cPacket*);
		cPacket* initiateTransmission();
		void doContention(simtime_t idleDuration);
		void txSuccess();
		void txFailure();

		boost::optional<simtime_t> getNextEventSlot();

		void setQueueSize(std::size_t size);
		long getBackoffTimes() const;
		long getBackoffSlots() const;
		long getInternalContentions() const;

	private:
		const CarrierSensing& mCarrierSensing;
		std::map<edca::AccessCategory, EdcaQueue, std::greater<edca::AccessCategory>> mQueues;
		edca::AccessCategory mLastAC;
};

#endif /* EDCA_H_ */
