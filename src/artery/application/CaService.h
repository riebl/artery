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

#ifndef ARTERY_CASERVICE_H_
#define ARTERY_CASERVICE_H_

#include "artery/application/ItsG5BaseService.h"
#include "artery/application/NetworkInterface.h"
#include "artery/utility/Geometry.h"
#include <vanetza/asn1/cam.hpp>
#include <vanetza/btp/data_interface.hpp>
#include <vanetza/units/angle.hpp>
#include <vanetza/units/velocity.hpp>
#include <omnetpp/simtime.h>

namespace artery
{

class Timer;
class VehicleDataProvider;

class CaService : public ItsG5BaseService
{
	public:
		CaService();
		void initialize() override;
		void indicate(const vanetza::btp::DataIndication&, std::unique_ptr<vanetza::UpPacket>, NetworkInterface& interface) override;
		void trigger() override;

	private:
		void checkTriggeringConditions(const omnetpp::SimTime&);
		bool checkHeadingDelta() const;
		bool checkPositionDelta() const;
		bool checkSpeedDelta() const;
		void sendCam(const omnetpp::SimTime&);
		omnetpp::SimTime genCamDcc();

		const VehicleDataProvider* mVehicleDataProvider;
		const Timer* mTimer;
		artery::LocalDynamicMap* mLocalDynamicMap;
		omnetpp::SimTime mGenCamMin;
		omnetpp::SimTime mGenCamMax;
		omnetpp::SimTime mGenCam;
		unsigned mGenCamLowDynamicsCounter;
		unsigned mGenCamLowDynamicsLimit;
		Position mLastCamPosition;
		vanetza::units::Velocity mLastCamSpeed;
		vanetza::units::Angle mLastCamHeading;
		omnetpp::SimTime mLastCamTimestamp;
		omnetpp::SimTime mLastLowCamTimestamp;
		vanetza::units::Angle mHeadingDelta;
		vanetza::units::Length mPositionDelta;
		vanetza::units::Velocity mSpeedDelta;
		bool mDccRestriction;
		bool mFixedRate;
};

vanetza::asn1::Cam createCooperativeAwarenessMessage(const VehicleDataProvider&, uint16_t genDeltaTime);
void addLowFrequencyContainer(vanetza::asn1::Cam&);

} // namespace artery

#endif /* ARTERY_CASERVICE_H_ */
