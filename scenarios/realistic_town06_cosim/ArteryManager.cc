// STD
#include <fstream>
#include <string>
#include <memory>

// Artery
#include <artery/traci/VehicleController.h>
#include <omnetpp/cpacket.h>
#include <vanetza/btp/data_request.hpp>
#include <vanetza/dcc/profile.hpp>
#include <vanetza/geonet/interface.hpp>

// plog
#include <plog/Log.h>
#include <plog/Severity.h>
#include <plog/Initializers/RollingFileInitializer.h>

// proto
#include <cavise/artery.pb.h>
#include <cavise/opencda.pb.h>
#include <google/protobuf/util/json_util.h>

// local
#include "ArteryManager.h"
#include "artery/application/NetworkInterface.h"
#include "comms/CommunicationManager.h"
#include "comms/SingletonHolder.h"

using namespace omnetpp;
using namespace vanetza;
using namespace artery;


static const simsignal_t scSignalCamReceived = cComponent::registerSignal("CamReceived");
Define_Module(ArteryManager)

ArteryManager::ArteryManager()
	: selfMessage_(nullptr)
{}

ArteryManager::~ArteryManager() {
	cancelAndDelete(selfMessage_);
}

void ArteryManager::indicate(const btp::DataIndication& ind, cPacket* packet, const NetworkInterface& net) {
    Enter_Method("indicate");

	PLOG(plog::debug) << "entering indicate()";

    if (packet->getByteLength() != 1024) {
		return;
	}
		
	PLOG(plog::debug) << "packet indication on channel " << net.channel << "\n";
	// Получаем данные из cPacket
	const char* json_c_str = packet->getName();
	int length = packet->getByteLength();
	
	structure_opencda::OpenCDA_message received_message;
	if (auto status = google::protobuf::util::JsonStringToMessage(json_c_str, &received_message); !status.ok()) {
		EV_ERROR << "Error parsing JSON: " << status.ToString() << std::endl;
	}

	const auto& vehicle = getFacilities().get_const<traci::VehicleController>();

	if (vehicle.getVehicleId().find("carla") != std::string::npos) {

		auto message = std::make_unique<structure_artery::Artery_message>();
		structure_artery::Artery_message::Received_information* received_info = message->add_received_information();

		received_info->set_artery_vid(vehicle.getVehicleId());

		std::string prefix = "carla";
		size_t pos = vehicle.getVehicleId().find(prefix);
		std::string numberStr = vehicle.getVehicleId().substr(pos + prefix.length());
		int vid = std::stoi(numberStr);

		auto& tp_cav = received_message.cav(vid).vid();

		received_info->set_vid(tp_cav);

		for(const auto& cav : received_message.cav()) {
			if (cav.vid() != received_info->vid()) {
				// Создаем новый объект Cav для сообщения Artery_message
				structure_artery::Artery_message::Received_information::Cav* new_cav = received_info->add_cav();
				// Заполняем новый объект значениями из объекта cav
				new_cav->set_vid(cav.vid());
				new_cav->set_ego_spd(cav.ego_spd());
				
				// Заполняем EgoPos
				structure_artery::Artery_message::Received_information::Cav::EgoPos* new_ego_pos = new_cav->mutable_ego_pos();
				new_ego_pos->set_x(cav.ego_pos().x());
				new_ego_pos->set_y(cav.ego_pos().y());
				new_ego_pos->set_z(cav.ego_pos().z());
				new_ego_pos->set_pitch(cav.ego_pos().pitch());
				new_ego_pos->set_yaw(cav.ego_pos().yaw());
				new_ego_pos->set_roll(cav.ego_pos().roll());
				
				// Заполняем BlueVehicles
				for (const auto& blue_cav : cav.blue_vehicles().blue_cav()) {
					structure_artery::Artery_message::Received_information::Cav::BlueVehicles::BlueCav* new_blue_cav = new_cav->mutable_blue_vehicles()->add_blue_cav();
					new_blue_cav->set_vid(blue_cav.vid());
					new_blue_cav->set_ego_spd(blue_cav.ego_spd());
					
					// Заполняем EgoPos для BlueCav
					structure_artery::Artery_message::Received_information::Cav::EgoPos* new_blue_ego_pos = new_blue_cav->mutable_ego_pos();
					new_blue_ego_pos->set_x(blue_cav.ego_pos().x());
					new_blue_ego_pos->set_y(blue_cav.ego_pos().y());
					new_blue_ego_pos->set_z(blue_cav.ego_pos().z());
					new_blue_ego_pos->set_pitch(blue_cav.ego_pos().pitch());
					new_blue_ego_pos->set_yaw(blue_cav.ego_pos().yaw());
					new_blue_ego_pos->set_roll(blue_cav.ego_pos().roll());
					
				}
				
				// Заполняем Vehicles
				for (const auto& cav_pos : cav.vehicles().cav_pos()) {
					structure_artery::Artery_message::Received_information::Cav::Vehicles::CavPos* new_cav_pos = new_cav->mutable_vehicles()->add_cav_pos();
					new_cav_pos->set_x(cav_pos.x());
					new_cav_pos->set_y(cav_pos.y());
					new_cav_pos->set_z(cav_pos.z());
				}

				// Заполняем TrafficLights
				for (const auto& tf_pos : cav.traffic_lights().tf_pos()) {
					structure_artery::Artery_message::Received_information::Cav::TrafficLights::TfPos* new_tf_pos = new_cav->mutable_traffic_lights()->add_tf_pos();
					new_tf_pos->set_x(tf_pos.x());
					new_tf_pos->set_y(tf_pos.y());
					new_tf_pos->set_z(tf_pos.z());
				}

				// Заполняем StaticObjects
				for (const auto& obj_pos : cav.static_objects().obj_pos()) {
					structure_artery::Artery_message::Received_information::Cav::StaticObjects::ObjPos* new_obj_pos = new_cav->mutable_static_objects()->add_obj_pos();
					new_obj_pos->set_x(obj_pos.x());
					new_obj_pos->set_y(obj_pos.y());
					new_obj_pos->set_z(obj_pos.z());
				}

				// Заполняем from_who_received
				for (const auto& from_who : cav.from_who_received()) {
					new_cav->add_from_who_received(from_who);
				}
				
			}
		}

		// google::protobuf::util::JsonOptions options;
		// options.add_whitespace = true; 
		// options.always_print_primitive_fields = true;
		// std::string json;

		// google::protobuf::util::MessageToJsonString(*message, &json, options);
		// PLOG(plog::debug) << "JSON: " << json << "\n";

		if(auto result = communicationManager_->push(vid, std::move(message)); result.isError()) {
			EV_ERROR << "Error while adding artery message to the queue: " << result.error() << "\n";
			PLOG(plog::error) << "Error while adding artery message to the queue: " << result.error() << "\n";
		}
	}
    
    delete(packet);
}

void ArteryManager::initialize() {
	if (!logInitalized_) {
		plog::init(plog::Severity::debug, "simdata/artery/logs/log.txt", 1024 * 1024, 2);
		EV << "Initalized logging, recording manager activity";
		PLOG(plog::info) << "Initalized logging, recording manager activity";
		logInitalized_ = true;
	}

	ItsG5Service::initialize();
	subscribe(scSignalCamReceived);

	if (auto holder = cavise::SingletonHolder<std::shared_ptr<cavise::CommunicationManager>>(); !holder.initalized()) {
		communicationManager_ = cavise::CommunicationManager::create("tcp://*:7777", 1024);
		communicationManager_->initialize();
		holder.initialize(communicationManager_);
	} else {
		communicationManager_ = holder.getInstance();
	}
	PLOG(plog::info) << "Initalized Communication Manager";

	selfMessage_ = new cMessage("Artery Manager");
	scheduleAt(simTime() + delta_, selfMessage_);
}

void ArteryManager::finish() {
	// you could record some scalars at this point
	ItsG5Service::finish();
}

void ArteryManager::handleMessage(cMessage* msg) {
	Enter_Method("handleMessage");
	if (msg == selfMessage_) {
		EV_INFO << "self message\n";
	}
}

void ArteryManager::trigger() {
	Enter_Method("trigger");

	PLOG(plog::debug) << "entering trigger()";

	// use an ITS-AID reserved for testing purposes
	static const vanetza::ItsAid example_its_aid = 16480;

	const auto& mco = getFacilities().get_const<MultiChannelPolicy>();
	const auto& networks = getFacilities().get_const<NetworkInterfaceTable>();

	for (const auto& channel : mco.allChannels(example_its_aid)) {
		std::shared_ptr<NetworkInterface> network = networks.select(channel);
		if (!network) {
			EV_ERROR << "No network interface available for channel " << channel << "\n";
		}

		btp::DataRequestB req;
		// use same port number as configured for listening on this channel
		req.destination_port = host_cast(getPortNumber(channel));
		req.gn.transport_type = geonet::TransportType::SHB;
		req.gn.traffic_class.tc_id(static_cast<unsigned int>(dcc::Profile::DP3));
		req.gn.communication_profile = geonet::CommunicationProfile::ITS_G5;
		req.gn.its_aid = example_its_aid;

		const auto& vehicle = getFacilities().get_const<traci::VehicleController>();
		if (vehicle.getVehicleId().find("carla") != std::string::npos) {

			std::unique_ptr<structure_opencda::OpenCDA_message> message;
			if (auto result = communicationManager_->collect(); result.isError()) {
				EV_ERROR << "error acquiring message: " << result.error() << "\n";
			} else {
				message = std::move(result.result());
			}

			for (const auto& cav : message->cav()) {
				PLOG(plog::debug) << "Cav vid: " + cav.vid() + '\n';
				EV_INFO << "Cav vid: " << cav.vid() << '\n';
			}
			PLOG(plog::debug) << "Cav number: " << message->cav_size() << '\n';
			EV_INFO << "Cav number: " << message->cav_size() << '\n';

			if (message->cav_size() > 0) {
				google::protobuf::util::JsonPrintOptions options;
				options.add_whitespace = true;
				options.always_print_primitive_fields = true;

				std::string json;
				google::protobuf::util::MessageToJsonString(*message, &json);

				const char *json_c_str = json.c_str();
				cPacket *packet = new cPacket(json_c_str);
				packet->setByteLength(1024);

				request(req, packet, network.get());
			}
		}
		cPacket *packet = new cPacket("Empty");
		packet->setByteLength(1);

		request(req, packet, network.get());
	}
}

void ArteryManager::receiveSignal(cComponent* source, simsignal_t signal, cObject*, cObject*) {
	Enter_Method("receiveSignal");

	if (signal == scSignalCamReceived) {
		const auto& vehicle = getFacilities().get_const<traci::VehicleController>();
		EV_INFO << "Vehicle " << vehicle.getVehicleId() << " received a CAM in sibling service\n";
	}
}
