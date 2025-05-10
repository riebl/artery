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

structure_artery::NDArray ConvertNDArray(const structure_opencda::NDArray& src) {
    structure_artery::NDArray dst;
    dst.set_dtype(src.dtype());
    dst.mutable_shape()->CopyFrom(src.shape());
    dst.set_data(src.data());
    return dst;
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

	std::string id = vehicle.getVehicleId();
	PLOG(plog::debug) << "Vehicle ID: " << id;
	
	if (!(id.rfind("rsu", 0) == 0 || id.rfind("cav", 0) == 0 || id.rfind("platoon", 0) == 0)) {
		PLOG(plog::debug) << "Skipping vehicle '" << id << "' — not RSU/CAV/Platoon";
		delete packet;
		return;
	}
	
	PLOG(plog::debug) << "Vehicle '" << id << "' passed prefix check";
	
	auto message = std::make_unique<structure_artery::Artery_message>();
	auto* received_info = message->add_received_information();
	received_info->set_id(id);
	
	std::string matchedPrefix;
	for (const auto& prefix : {"rsu", "cav", "platoon"}) {
		if (id.rfind(prefix, 0) == 0) {
			matchedPrefix = prefix;
			PLOG(plog::debug) << "Matched prefix: " << matchedPrefix;
			break;
		}
	}
	
	if (matchedPrefix.empty()) {
		PLOG(plog::warning) << "No valid prefix found in vehicle ID: " << id;
		delete packet;
		return;
	}

	for (const auto& entity : received_message.entity()) {
		PLOG(plog::debug) << "  - entity.id() = " << entity.id();
	}
	
	for (const auto& entity : received_message.entity()) {
		PLOG(plog::debug) << "Processing entity with ID: " << entity.id();
	
		if (entity.id() != received_info->id()) {
			PLOG(plog::debug) << "Adding new entity to received_info: " << entity.id();
			auto* new_entity = received_info->add_entity();
			new_entity->set_id(entity.id());
	
			if (entity.has_infra()) {
				new_entity->set_infra(entity.infra());
			}
			if (entity.has_velocity()) {
				new_entity->set_velocity(entity.velocity());
			}
			if (entity.has_time_delay()) {
				new_entity->set_time_delay(entity.time_delay());
			}
	
			new_entity->mutable_object_ids()->CopyFrom(entity.object_ids());
			new_entity->mutable_lidar_pose()->CopyFrom(entity.lidar_pose());
	
			if (entity.has_object_bbx_center()) {
				new_entity->mutable_object_bbx_center()->CopyFrom(ConvertNDArray(entity.object_bbx_center()));
			}
			if (entity.has_object_bbx_mask()) {
				new_entity->mutable_object_bbx_mask()->CopyFrom(ConvertNDArray(entity.object_bbx_mask()));
			}
			if (entity.has_anchor_box()) {
				new_entity->mutable_anchor_box()->CopyFrom(ConvertNDArray(entity.anchor_box()));
			}
			if (entity.has_pos_equal_one()) {
				new_entity->mutable_pos_equal_one()->CopyFrom(ConvertNDArray(entity.pos_equal_one()));
			}
			if (entity.has_neg_equal_one()) {
				new_entity->mutable_neg_equal_one()->CopyFrom(ConvertNDArray(entity.neg_equal_one()));
			}
			if (entity.has_targets()) {
				new_entity->mutable_targets()->CopyFrom(ConvertNDArray(entity.targets()));
			}
			if (entity.has_origin_lidar()) {
				new_entity->mutable_origin_lidar()->CopyFrom(ConvertNDArray(entity.origin_lidar()));
			}
			if (entity.has_spatial_correction_matrix()) {
				new_entity->mutable_spatial_correction_matrix()->CopyFrom(ConvertNDArray(entity.spatial_correction_matrix()));
			}
			if (entity.has_voxel_num_points()) {
				new_entity->mutable_voxel_num_points()->CopyFrom(ConvertNDArray(entity.voxel_num_points()));
			}
			if (entity.has_voxel_features()) {
				new_entity->mutable_voxel_features()->CopyFrom(ConvertNDArray(entity.voxel_features()));
			}
			if (entity.has_voxel_coords()) {
				new_entity->mutable_voxel_coords()->CopyFrom(ConvertNDArray(entity.voxel_coords()));
			}
			if (entity.has_projected_lidar()) {
				new_entity->mutable_projected_lidar()->CopyFrom(ConvertNDArray(entity.projected_lidar()));
			}
	
			PLOG(plog::debug) << "Finished adding entity " << entity.id();
		} else {
			PLOG(plog::debug) << "Skipping own entity ID: " << entity.id();
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

	if(auto result = communicationManager_->push(id, std::move(message)); result.isError()) {
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
		std::string id = vehicle.getVehicleId();
		
		if (!(id.rfind("rsu", 0) == 0 || id.rfind("cav", 0) == 0 || id.rfind("platoon", 0) == 0)) {
			auto packet = new Empty();
			packet->setSender(id.c_str());
			request(req, std::move(packet), network.get());
			return;
		}

		std::unique_ptr<structure_opencda::OpenCDA_message> message;
		if (auto result = communicationManager_->collect(); result.isError()) {
			PLOG(plog::debug) << "error acquiring message: " << result.error();
		} else {
			message = std::move(result.result());
		}

		PLOG(plog::debug) << "Amount of CAVs and RSUs: " << message->entity_size() << '\n';
		for (const auto& entity : message->entity()) {
			PLOG(plog::debug) << "Entity id: " << entity.id();
		}

		if (message->entity_size() > 0) {
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
