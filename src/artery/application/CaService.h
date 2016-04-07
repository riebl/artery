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

#ifndef CASERVICE_H_
#define CASERVICE_H_

#include "artery/application/ItsG5BaseService.h"
#include "veins/base/utils/Coord.h"
#include <vanetza/asn1/cam.hpp>
#include <vanetza/btp/data_interface.hpp>
#include <vanetza/units/angle.hpp>
#include <vanetza/units/velocity.hpp>
#include <simtime_t.h>


class CaService : public ItsG5BaseService
{
	public:
		CaService();
		void indicate(const vanetza::btp::DataIndication&, std::unique_ptr<vanetza::UpPacket>) override;
		void trigger() override;

	private:
		void checkTriggeringConditions(const VehicleDataProvider&, const simtime_t&);
		void sendCam(const VehicleDataProvider&, const simtime_t&);
		simtime_t genCamDcc();

		const simtime_t mGenCamMin;
		const simtime_t mGenCamMax;
		simtime_t mGenCam;
		unsigned mGenCamLowDynamicsCounter;
		unsigned mGenCamLowDynamicsLimit;
		Coord mLastCamPosition;
		vanetza::units::Velocity mLastCamSpeed;
		vanetza::units::Angle mLastCamHeading;
		simtime_t mLastCamTimestamp;
		simtime_t mLastLowCamTimestamp;
};

vanetza::asn1::Cam createCooperativeAwarenessMessage(const VehicleDataProvider&);
void addLowFrequencyContainer(vanetza::asn1::Cam&);

#endif /*CASERVICE_H_ */
