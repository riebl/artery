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
#include "artery/application/LocalDynamicMap.h"
#include "artery/application/Timer.h"
#include "artery/application/VehicleDataProvider.h"
#include "artery/utility/Identity.h"
#include "veins/base/modules/BaseApplLayer.h"
#include <omnetpp.h>
#include <vanetza/access/data_request.hpp>
#include <vanetza/access/interface.hpp>
#include <vanetza/btp/data_interface.hpp>
#include <vanetza/btp/port_dispatcher.hpp>
#include <vanetza/common/clock.hpp>
#include <vanetza/common/runtime.hpp>
#include <vanetza/dcc/flow_control.hpp>
#include <vanetza/dcc/scheduler.hpp>
#include <vanetza/dcc/state_machine.hpp>
#include <vanetza/geonet/packet.hpp>
#include <vanetza/geonet/router.hpp>
#include <map>
#include <memory>

// forward declarations
class ItsG5BaseService;
class RadioDriverBase;
namespace traci { class VehicleController; }

/**
 * Middleware providing a runtime context for services.
 */
class ItsG5Middleware :
    public cSimpleModule, public cListener,
    public vanetza::access::Interface, public vanetza::btp::RequestInterface
{
	public:
		typedef uint16_t port_type;

		ItsG5Middleware();
		void request(const vanetza::access::DataRequest&, std::unique_ptr<vanetza::DownPacket>) override;
		void request(const vanetza::btp::DataRequestB&, std::unique_ptr<vanetza::DownPacket>) override;
		Facilities* getFacilities() { return &mFacilities; }
		port_type getPortNumber(const ItsG5BaseService*) const;

	protected:
		void initialize(int stage) override;
		int numInitStages() const override;
		void finish() override;
		void handleMessage(cMessage *msg) override;
		virtual void handleSelfMsg(cMessage *msg);
		virtual void handleLowerMsg(cMessage *msg);
		void receiveSignal(cComponent*, simsignal_t, cObject*, cObject*) override;
		void receiveSignal(cComponent*, simsignal_t, double, cObject*) override;

	private:
		cModule* findHost();
		void update();
		void updatePosition();
		void updateServices();
		void initializeMiddleware();
		void initializeServices();
		void initializeVehicleController();
		bool checkServiceFilterRules(const cXMLElement* filters) const;
		void scheduleRuntime();
		SimTime convertSimTime(vanetza::Clock::time_point tp) const;

		//Veins::TraCIMobility* mMobility;
		RadioDriverBase* mRadioDriver;
		cGate* mRadioDriverIn;
		cGate* mRadioDriverOut;
		traci::VehicleController* mVehicleController;
		VehicleDataProvider mVehicleDataProvider;
		Timer mTimer;
		artery::Identity mIdentity;
		artery::LocalDynamicMap mLocalDynamicMap;
		vanetza::Runtime mRuntime;
		vanetza::dcc::StateMachine mDccFsm;
		vanetza::dcc::Scheduler mDccScheduler;
		std::unique_ptr<vanetza::dcc::FlowControl> mDccControl;
		vanetza::geonet::MIB mGeoMib;
		std::unique_ptr<vanetza::geonet::Router> mGeoRouter;
		vanetza::btp::PortDispatcher mBtpPortDispatcher;
		boost::posix_time::ptime mTimebase;
		simtime_t mUpdateInterval;
		cMessage* mUpdateMessage;
		cMessage* mUpdateRuntimeMessage;
		Facilities mFacilities;
		std::map<ItsG5BaseService*, port_type> mServices;
};

#endif
