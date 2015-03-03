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

#ifndef ITSG5MIDDLEWARE_H_
#define ITSG5MIDDLEWARE_H_

#include "artery/application/Facilities.h"
#include "artery/application/VehicleDataProvider.h"
#include "veins/base/modules/BaseApplLayer.h"
#include <omnetpp.h>
#include <vanetza/access/data_request.hpp>
#include <vanetza/access/interface.hpp>
#include <vanetza/btp/data_interface.hpp>
#include <vanetza/btp/port_dispatcher.hpp>
#include <vanetza/common/clock.hpp>
#include <vanetza/dcc/access_control.hpp>
#include <vanetza/dcc/scheduler.hpp>
#include <vanetza/dcc/state_machine.hpp>
#include <vanetza/geonet/packet.hpp>
#include <vanetza/geonet/router.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <map>
#include <memory>

// forward declarations
namespace Veins { class TraCIMobility; }
class ItsG5BaseService;

/**
 * Middleware providing a runtime context for services.
 * It can be plugged in wherever a IBaseApplLayer implementation is required.
 */
class ItsG5Middleware : public BaseApplLayer, public vanetza::access::Interface, public vanetza::btp::RequestInterface
{
	public:
		typedef uint16_t port_type;

		ItsG5Middleware();
		void request(const vanetza::access::DataRequest&, std::unique_ptr<vanetza::geonet::DownPacket>) override;
		void request(const vanetza::btp::DataRequestB&, std::unique_ptr<vanetza::btp::DownPacket>) override;
		Facilities* getFacilities() { return mFacilities.get(); }
		port_type getPortNumber(const ItsG5BaseService*) const;

	protected:
		void initialize(int stage) override;
		int numInitStages() const override;
		void finish() override;
		void handleMessage(cMessage *msg) override;
		void handleSelfMsg(cMessage *sg) override;
		void handleLowerMsg(cMessage *msg) override;
		void handleLowerControl(cMessage *msg) override;
		void receiveSignal(cComponent*, simsignal_t, cObject*) override;

	private:
		void update();
		void updateGeoRouter();
		void updateServices();
		void initializeMiddleware();
		void initializeServices();
		bool checkServiceFilterRules(const cXMLElement* filters) const;
		vanetza::geonet::Timestamp deriveTimestamp(simtime_t) const;

		Veins::TraCIMobility* mMobility;
		VehicleDataProvider mVehicleDataProvider;
		vanetza::clock::time_point mClock;
		vanetza::dcc::StateMachine mDccFsm;
		vanetza::dcc::Scheduler mDccScheduler;
		vanetza::dcc::AccessControl mDccControl;
		vanetza::geonet::MIB mGeoMib;
		vanetza::geonet::Router mGeoRouter;
		vanetza::btp::PortDispatcher mBtpPortDispatcher;
		boost::posix_time::ptime mTimebase;
		unsigned mAdditionalHeaderBits;
		simtime_t mUpdateInterval;
		cMessage* mUpdateMessage;
		std::unique_ptr<Facilities> mFacilities;
		std::map<ItsG5BaseService*, port_type> mServices;
};

#endif
