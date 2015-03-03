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

#ifndef CARRIERSENSING_H_
#define CARRIERSENSING_H_

#include <simtime_t.h>
#include <boost/optional/optional.hpp>

class CarrierSensing
{
	public:
		enum State {
			IDLE, BUSY_PHYSICAL, BUSY_SELF
		};

		CarrierSensing();
		virtual ~CarrierSensing();
		void setState(State);
		State getState() const { return mState; }
		simtime_t getStateSince() const { return mSince; }
		bool isBusy() const { return !isIdle(); }
		bool isIdle() const { return mState == IDLE; }
		boost::optional<simtime_t> getIdleDuration() const;

	private:
		simtime_t mSince;
		State mState;
};

#endif /* CARRIERSENSING_H_ */
