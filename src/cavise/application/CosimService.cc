// STD
#include <memory>
#include <string>
#include <cstddef>

// Artery
#include <omnetpp/cpacket.h>
#include <vanetza/dcc/profile.hpp>
#include <vanetza/net/osi_layer.hpp>
#include <vanetza/btp/data_request.hpp>
#include <vanetza/net/packet_variant.hpp>
#include <vanetza/geonet/interface.hpp>
#include <artery/traci/VehicleController.h>

// plog
#include <plog/Log.h>
#include <plog/Severity.h>
#include <plog/Initializers/RollingFileInitializer.h>

// proto
#include <cavise/artery.pb.h>
#include <cavise/opencda.pb.h>
#include <google/protobuf/util/json_util.h>

// omnetpp messages
#include <cavise_msgs/Opencda_m.h>

// communication
#include <cavise/Init.h>
#include <cavise/comms/SingletonHolder.h>
#include <cavise/comms/CommunicationManager.h>

// local
#include <cavise/application/CosimService.h>

#define STRINGIFY(class) #class

using namespace cavise;

namespace {
    static const omnetpp::simsignal_t scSignalCamReceived = omnetpp::cComponent::registerSignal("CamReceived");
}

Define_Module(CosimService)

CosimService::CosimService() {
	cavise::init();
}

void CosimService::indicate(const vanetza::btp::DataIndication& /* ind */, omnetpp::cPacket* packet, const artery::NetworkInterface& /* interface */) {
	CAVISE_STUB();
    
	const auto& vehicle = getFacilities().get_const<traci::VehicleController>();
	if (!strcmp(packet->getClassName(), STRINGIFY(Empty))) {
		PLOG(plog::debug) << "receiving message " << STRINGIFY(Empty) << " on vehicle " << vehicle.getVehicleId();
		delete packet;
		return;
	}
	if (!strcmp(packet->getClassName(), STRINGIFY(OpencdaPayload))) {
		PLOG(plog::debug) << "receiving message " << STRINGIFY(OpencdaPayload) << " on vehicle " << vehicle.getVehicleId();
	} else {
		PLOG(plog::warning) << "receiving unknown message " << packet->getClassName() << " on vehicle " << vehicle.getVehicleId();
		delete packet;
		return;
	}

	OpencdaPayload* payload = static_cast<OpencdaPayload*>(packet);
	structure_opencda::OpenCDA_message received_message;
	if (auto status = google::protobuf::util::JsonStringToMessage(payload->getJson(), &received_message); !status.ok()) {
		PLOG(plog::debug) << "error parsing JSON: " << status.ToString();
	}

	if (vehicle.getVehicleId().find("carla") == std::string::npos) {
		return;
	}

	auto message = std::make_unique<structure_artery::Artery_message>();
	structure_artery::Artery_message::Received_information* received_info = message->add_received_information();

	received_info->set_artery_vid(vehicle.getVehicleId());

	std::string prefix = "carla";
	size_t pos = vehicle.getVehicleId().find(prefix);
	std::string numberStr = vehicle.getVehicleId().substr(pos + prefix.length());
	int vid = std::stoi(numberStr);

	auto& tp_cav = received_message.cav(vid).vid();

	received_info->set_vid(tp_cav);

	for (const auto& cav : received_message.cav()) {
		if (cav.vid() != received_info->vid()) {
			// Создаем новый объект Cav для сообщения Artery_message
			structure_artery::Artery_message::Received_information::Cav* new_cav = received_info->add_cav();
			// Заполняем новый объект значениями из объекта cav
			new_cav->set_vid(cav.vid());
			new_cav->set_ego_spd(cav.ego_spd());	
		}
	}

	google::protobuf::util::JsonOptions options;
	options.add_whitespace = true; 
	options.always_print_primitive_fields = true;

	std::string json;
	if (auto status = google::protobuf::util::MessageToJsonString(*message, &json, options); status.ok()) {
		PLOG(plog::debug) << "JSON: " << json;
	} else {
		PLOG(plog::warning) << "failed to serialize to json: " << json;
	}

	if(auto result = communicationManager_->push(vid, std::move(message)); result.isError()) {
		PLOG(plog::error) << "Error while adding artery message to the queue: " << result.error();
	}

	delete packet;
}

void CosimService::initialize() {
    CAVISE_STUB();

	ItsG5Service::initialize();
	subscribe(scSignalCamReceived);
	if (auto holder = cavise::SingletonHolder<std::shared_ptr<CommunicationManager>>(); !holder.initalized()) {
		communicationManager_ = CommunicationManager::create("tcp://*:7777", 1024);
		communicationManager_->initialize();
		holder.initialize(communicationManager_);
        PLOG(plog::info) << "object " << this << " initialized communication handler instance";
	} else {
		communicationManager_ = holder.getInstance();
	    PLOG(plog::debug) << "object " << this << " acquired communication handler instance";
	}
}

void CosimService::trigger() {
    CAVISE_STUB();

	static const vanetza::ItsAid example_its_aid = 16480;
	const auto& mco = getFacilities().get_const<artery::MultiChannelPolicy>();
	const auto& networks = getFacilities().get_const<artery::NetworkInterfaceTable>();

	for (const auto& channel : mco.allChannels(example_its_aid)) {
		std::shared_ptr<artery::NetworkInterface> network = networks.select(channel);
		if (!network) {
			PLOG(plog::warning) << "no network interface available for channel " << channel << "\n";
		}

		vanetza::btp::DataRequestB req;
		// use same port number as configured for listening on this channel
		req.destination_port = vanetza::host_cast(getPortNumber(channel));
		req.gn.transport_type = vanetza::geonet::TransportType::SHB;
		req.gn.traffic_class.tc_id(static_cast<unsigned int>(vanetza::dcc::Profile::DP3));
		req.gn.communication_profile = vanetza::geonet::CommunicationProfile::ITS_G5;
		req.gn.its_aid = example_its_aid;

		const auto& vehicle = getFacilities().get_const<traci::VehicleController>();
		if (vehicle.getVehicleId().find("carla") == std::string::npos) {
			auto packet = new Empty();
			packet->setSender(vehicle.getVehicleId().c_str());
			request(req, std::move(packet), network.get());
			return;
		}

		std::unique_ptr<structure_opencda::OpenCDA_message> message;
		if (auto result = communicationManager_->collect(); result.isError()) {
			PLOG(plog::debug) << "error acquiring message: " << result.error();
		} else {
			message = std::move(result.result());
		}

		PLOG(plog::debug) << "Cav number: " << message->cav_size() << '\n';
		for (const auto& cav : message->cav()) {
			PLOG(plog::debug) << "Cav vid: " + cav.vid() + '\n';
		}

		if (message->cav_size() > 0) {
			google::protobuf::util::JsonPrintOptions options;
			options.add_whitespace = true;
			options.always_print_primitive_fields = true;

			std::string json;
			if (auto status = google::protobuf::util::MessageToJsonString(*message, &json); status.ok()) {
				auto packet = new OpencdaPayload();
				packet->setJson(json.c_str());
				packet->setSender(vehicle.getVehicleId().c_str());
				request(req, packet, network.get());
				continue;
			} else {
				PLOG(plog::debug) << "failed to serialize opencda payload: " << status.ToString();
			}
		}
	}
}

void CosimService::receiveSignal(cComponent* /* source */, omnetpp::simsignal_t signal, cObject* /* obj1 */, cObject* /* obj2 */) {
	CAVISE_STUB();
	if (signal == scSignalCamReceived) {
		const auto& vehicle = getFacilities().get_const<traci::VehicleController>();
		PLOG(plog::debug) << "vehicle " << vehicle.getVehicleId() << " received a CAM in sibling service";
	}
}
