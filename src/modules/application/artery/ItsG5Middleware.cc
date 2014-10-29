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

#include "AccessCategories.h"
#include "AccessCategoriesVanetza.h"
#include "ChannelAccess.h"
#include "ChannelLoadReport_m.h"
#include "GeoNetPacket_m.h"
#include "GeoNetToMacControlInfo.h"
#include "ItsG5Middleware.h"
#include "ItsG5Service.h"
#include "MacToGeoNetControlInfo.h"
#include "TraCIMobility.h"
#include <vanetza/btp/header.hpp>
#include <vanetza/btp/header_conversion.hpp>
#include <vanetza/btp/ports.hpp>
#include <vanetza/geonet/data_confirm.hpp>
#include <vanetza/geonet/packet.hpp>
#include <vanetza/geonet/position_vector.hpp>
#include <vanetza/net/mac_address.hpp>

Define_Module(ItsG5Middleware);

const simsignalwrap_t cMobilityStateChangedSignal(MIXIM_SIGNAL_MOBILITY_CHANGE_NAME);

LAddress::L2Type convertToL2Type(const vanetza::MacAddress& mac)
{
	if (mac == vanetza::cBroadcastMacAddress) {
		return LAddress::L2BROADCAST;
	} else {
		LAddress::L2Type result = 0;
		for (unsigned i = 0; i < mac.octets.size(); ++i) {
			result <<= 8;
			result |= mac.octets[i];
		}
		return result;
	}
}

vanetza::MacAddress convertToMacAddress(const LAddress::L2Type& addr)
{
	if (addr == LAddress::L2BROADCAST) {
		return vanetza::cBroadcastMacAddress;
	} else {
		LAddress::L2Type tmp = addr;
		vanetza::MacAddress mac;
		for (unsigned i = mac.octets.size(); i > 0; --i) {
			mac.octets[i - 1] = tmp & 0xff;
			tmp >>= 8;
		}
		return mac;
	}
}

ItsG5Middleware::ItsG5Middleware() :
		mClock(std::chrono::milliseconds(simTime().inUnit(SIMTIME_MS))),
		mDccScheduler(mDccFsm, mClock),
		mDccControl(mDccScheduler, *this),
		mGeoRouter(mGeoMib, mDccControl),
		mAdditionalHeaderBits(0)
{
}

void ItsG5Middleware::request(const vanetza::access::DataRequest& req,
		std::unique_ptr<vanetza::geonet::DownPacket> payload)
{
	Enter_Method_Silent();
	GeoNetToMacControlInfo* macCtrlInfo = new GeoNetToMacControlInfo();
	macCtrlInfo->access_category = edca::map(req.access_category);
	macCtrlInfo->destination_addr = convertToL2Type(req.destination_addr);
	macCtrlInfo->source_addr = convertToL2Type(req.source_addr);

	if ((*payload)[vanetza::OsiLayer::Network].ptr() == nullptr) {
		opp_error("Missing network layer payload in middleware request");
	}

	GeoNetPacket* net = new GeoNetPacket("GeoNet");
	net->setByteLength(payload->size());
	net->setPayload(GeoNetPacketWrapper(std::move(payload)));
	net->setControlInfo(macCtrlInfo);
	net->addBitLength(mAdditionalHeaderBits);

	sendDown(net);
}

void ItsG5Middleware::request(const vanetza::btp::DataRequestB& req, std::unique_ptr<vanetza::btp::DownPacket> payload)
{
	Enter_Method("request");
	using namespace vanetza;
	btp::HeaderB btp_header;
	btp_header.destination_port = req.destination_port;
	btp_header.destination_port_info = req.destination_port_info;
	payload->layer(OsiLayer::Transport) = btp_header;

	switch (req.gn.transport_type) {
		case geonet::TransportType::SHB: {
			geonet::ShbDataRequest request(mGeoMib);
			request.upper_protocol = geonet::UpperProtocol::BTP_B;
			request.communication_profile = req.gn.communication_profile;
			if (req.gn.maximum_lifetime) {
				request.maximum_lifetime = req.gn.maximum_lifetime.get();
			}
			request.repetition = req.gn.repetition;
			request.traffic_class = req.gn.traffic_class;
			mGeoRouter.request(request, std::move(payload));
		}
			break;
		default:
			opp_error("Unknown or unimplemented transport type");
			break;
	}
}

int ItsG5Middleware::numInitStages() const
{
	return std::max(BaseApplLayer::numInitStages(), 3);
}

void ItsG5Middleware::initialize(int stage)
{
	BaseApplLayer::initialize(stage);
	switch (stage) {
		case 0:
			initializeMiddleware();
			break;
		case 1:
			initializeServices();
			break;
		case 2: {
				// start update cycle with random jitter to avoid unrealistic node synchronization
				auto start = simTime() + mUpdateInterval + uniform(SimTime(0, SIMTIME_MS), mUpdateInterval);
				scheduleAt(start, mUpdateMessage);
			}
			break;
		default:
			break;
	}
}

void ItsG5Middleware::initializeMiddleware()
{
	mMobility = Veins::TraCIMobilityAccess().get(getParentModule());
	if (mMobility == nullptr) {
		opp_error("Mobility not found");
	}
	mFacilities.reset(new Facilities(mVehicleDataProvider, *mMobility, mDccFsm, mDccScheduler));

	mAdditionalHeaderBits = par("headerLength");
	mTimebase = boost::posix_time::time_from_string(par("datetime"));
	mUpdateInterval = par("updateInterval").doubleValue();
	mUpdateMessage = new cMessage("update ITS-G5");
	findHost()->subscribe(cMobilityStateChangedSignal, this);

	vanetza::geonet::Address gn_addr;
	gn_addr.is_manually_configured(true);
	gn_addr.station_type(vanetza::geonet::StationType::PASSENGER_CAR);
	gn_addr.country_code(0);
	gn_addr.mid(vanetza::create_mac_address(this->getId()));
	mGeoRouter.set_address(gn_addr);
	mGeoRouter.set_time(deriveTimestamp(simTime()));

	using vanetza::geonet::UpperProtocol;
	mGeoRouter.set_transport_handler(UpperProtocol::BTP_B, mBtpPortDispatcher);
}

void ItsG5Middleware::initializeServices()
{
	cXMLElement* config = par("services").xmlValue();
	for (cXMLElement* service_cfg : config->getChildrenByTagName("service")) {
		cModuleType* module_type = cModuleType::get(service_cfg->getAttribute("type"));
		cModule* module = module_type->createScheduleInit(service_cfg->getAttribute("name"), this);

		ItsG5BaseService* service = dynamic_cast<ItsG5BaseService*>(module);
		if (service == nullptr) {
			opp_error("%s is not of type ItsG5BaseService", service_cfg->getAttribute("type"));
		} else {
			int port = strtol(service_cfg->getFirstChildWithTag("port")->getNodeValue(), nullptr, 0);
			mServices.emplace(service, port);
			mBtpPortDispatcher.set_non_interactive_handler(vanetza::host_cast<uint16_t>(port), service);
		}
	}
}

void ItsG5Middleware::finish()
{
	cancelAndDelete(mUpdateMessage);
	findHost()->unsubscribe(cMobilityStateChangedSignal, this);
	BaseApplLayer::finish();
}

void ItsG5Middleware::handleMessage(cMessage *msg)
{
	// This clock has to be steady, but no time base required
	mClock = decltype(mClock) { std::chrono::milliseconds(simTime().inUnit(SIMTIME_MS)) };

	// Don't forget to dispatch message properly
	BaseApplLayer::handleMessage(msg);
}

void ItsG5Middleware::handleSelfMsg(cMessage *msg)
{
	if (msg == mUpdateMessage) {
		update();
	} else {
		opp_error("Unknown self-message in ITS-G5");
	}
}

void ItsG5Middleware::handleLowerMsg(cMessage *msg)
{
	auto* packet = dynamic_cast<GeoNetPacket*>(msg);
	assert(packet);
	auto& wrapper = packet->getPayload();
	auto* info = dynamic_cast<MacToGeoNetControlInfo*>(packet->getControlInfo());
	assert(info);
	vanetza::MacAddress sender = convertToMacAddress(info->source_addr);
	vanetza::MacAddress destination = convertToMacAddress(info->destination_addr);
	mGeoRouter.indicate(wrapper.extract_up_packet(), sender, destination);
	delete msg;
}

void ItsG5Middleware::handleLowerControl(cMessage *msg)
{
	auto* channel_load_msg = dynamic_cast<ChannelLoadReport*>(msg);
	if (nullptr != channel_load_msg) {
		mDccFsm.update(channel_load_msg->getChannelLoad());
	} else {
		opp_error("Unknown lower control message");
	}
	delete msg;
}

void ItsG5Middleware::update()
{
	updateGeoRouter();
	updateServices();
	scheduleAt(simTime() + mUpdateInterval, mUpdateMessage);
}

void ItsG5Middleware::receiveSignal(cComponent* component, simsignal_t signal, cObject* obj)
{
	BaseApplLayer::receiveSignal(component, signal, obj);
	if (signal == cMobilityStateChangedSignal) {
		mVehicleDataProvider.update(mMobility);
	}
}

void ItsG5Middleware::updateGeoRouter()
{
	mGeoRouter.update(deriveTimestamp(simTime()));

	vanetza::geonet::LongPositionVector lpv;
	lpv.timestamp = deriveTimestamp(mVehicleDataProvider.simtime());
	lpv.latitude = static_cast<decltype(lpv.latitude)>(mVehicleDataProvider.latitude());
	lpv.longitude = static_cast<decltype(lpv.longitude)>(mVehicleDataProvider.longitude());
	lpv.heading = static_cast<decltype(lpv.heading)>(mVehicleDataProvider.heading());
	lpv.speed = static_cast<decltype(lpv.speed)>(mVehicleDataProvider.speed());
	lpv.position_accuracy_indicator = true;
	mGeoRouter.update(lpv);
}

void ItsG5Middleware::updateServices()
{
	for (auto& kv : mServices) {
		kv.first->trigger();
	}
}

ItsG5Middleware::port_type ItsG5Middleware::getPortNumber(const ItsG5BaseService* service) const
{
	port_type port = 0;
	auto it = mServices.find(const_cast<ItsG5BaseService*>(service));
	if (it != mServices.end()) {
		port = it->second;
	}
	return port;
}

vanetza::geonet::Timestamp ItsG5Middleware::deriveTimestamp(simtime_t time) const
{
	boost::posix_time::ptime posix_time = mTimebase + boost::posix_time::milliseconds(time.inUnit(SIMTIME_MS));
	return vanetza::geonet::Timestamp(posix_time);
}
