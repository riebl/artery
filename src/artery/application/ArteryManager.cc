#include "ArteryManager.h"
#include "artery/traci/VehicleController.h"
#include <omnetpp/cpacket.h>
#include <vanetza/btp/data_request.hpp>
#include <vanetza/dcc/profile.hpp>
#include <vanetza/geonet/interface.hpp>

#include <fstream>
#include <string>
#include "OpenCDA_message_structure.pb.h"
#include "Artery_message_structure.pb.h"


using namespace omnetpp;
using namespace vanetza;

namespace artery
{


static const simsignal_t scSignalCamReceived = cComponent::registerSignal("CamReceived");

Define_Module(ArteryManager)

ArteryManager::ArteryManager()
{
}

ArteryManager::~ArteryManager()
{
	cancelAndDelete(m_self_msg);
}

void ArteryManager::indicate(const btp::DataIndication& ind, cPacket* packet, const NetworkInterface& net)
{
    Enter_Method("indicate");

    if (packet->getByteLength() == 42) {
        EV_INFO << "packet indication on channel " << net.channel << "\n";
    } 
	// Получаем вложенный пакет
	cPacket *innerPacket = packet->decapsulate();
	if (innerPacket) {
		// Получаем указатель на данные в виде байтового массива
		const char *serializedData = dynamic_cast<cPacket*>(innerPacket)->getName();
		int serializedDataLenght = dynamic_cast<cPacket*>(innerPacket)->getByteLength();
		// Здесь вы можете обработать данные, как вам нужно
		if (serializedData) {
			structure_opencda::OpenCDA_message received_message;

			if (!received_message.ParseFromArray(&serializedData, serializedDataLenght)) {
				EV_INFO << "Failed to parse received msg" << std::endl;
				throw cRuntimeError("Error while parsing file");
			}
			std::string Artery_filename = "Messages/Artery_message.proto";

			structure_artery::Artery_message artery_message;
			structure_artery::Artery_message::Received_information* received_information = artery_message.add_received_information();
			
			auto& vehicle = getFacilities().get_const<traci::VehicleController>();
			received_information->set_artery_vid(vehicle.getVehicleId());
			int veh_id = std::stoi(vehicle.getVehicleId().substr(vehicle.getVehicleId().length() - 1));

			received_information->set_vid(received_message.cav(veh_id).vid());
			
			int k = 0;
			for (const auto& cav : received_message.cav()) {
				if (k != veh_id)
				{
					structure_artery::Artery_message_Received_information_Cav* new_cav = received_information->add_cav();
					new_cav->set_vid(cav.vid()); // Assuming vid is a member variable of cav
				}
				k++;
			}

			std::string serialized_data;
			if (!artery_message.SerializeToString(&serialized_data)) {
				EV_ERROR << "Failed to serialize artery_message." << std::endl;
				throw cRuntimeError("Failed to serialize artery_message.");
			}

			std::ofstream outFile(Artery_filename, std::ios::out | std::ios::app | std::ios::binary);
			if (outFile.is_open()) {
				outFile.write(serialized_data.c_str(), serialized_data.length());
				outFile.close();
				EV_INFO << "Data written to " << Artery_filename << std::endl;
			} else {
				EV_ERROR << "Failed to open " << Artery_filename << " for writing." << std::endl;
				throw cRuntimeError("Failed to open file.");
			}

			int receivedInfoCount = artery_message.received_information_size();
			if (receivedInfoCount >= 3) {
				std::ofstream outFile(Artery_filename, std::ios::out | std::ios::binary | std::ios::trunc);
				if (!outFile.is_open()) {
					EV_ERROR << "Failed to open " << Artery_filename << " for truncation." << std::endl;
				}
				outFile.close();
			}

		}
		delete innerPacket; // Освобождаем память, так как мы более не нуждаемся во вложенном пакете
	}
    
    delete(packet);
}


void ArteryManager::initialize()
{
	ItsG5Service::initialize();
	m_self_msg = new cMessage("Artery Manager");
	subscribe(scSignalCamReceived);

	scheduleAt(simTime() + 3.0, m_self_msg);
}

void ArteryManager::finish()
{
	// you could record some scalars at this point
	ItsG5Service::finish();
}

void ArteryManager::handleMessage(cMessage* msg)
{
	Enter_Method("handleMessage");
	// EV_INFO << "Vehicle " << vehicle.getVehicleId() << " received a CAM in sibling serivce\n";
	if (msg == m_self_msg) {
		EV_INFO << "self message\n";
	}
}

void ArteryManager::trigger()
{
	Enter_Method("trigger");

	// use an ITS-AID reserved for testing purposes
	static const vanetza::ItsAid example_its_aid = 16480;

	auto& mco = getFacilities().get_const<MultiChannelPolicy>();
	auto& networks = getFacilities().get_const<NetworkInterfaceTable>();

	for (auto channel : mco.allChannels(example_its_aid)) {
		auto network = networks.select(channel);
		if (network) {
			btp::DataRequestB req;
			// use same port number as configured for listening on this channel
			req.destination_port = host_cast(getPortNumber(channel));
			req.gn.transport_type = geonet::TransportType::SHB;
			req.gn.traffic_class.tc_id(static_cast<unsigned>(dcc::Profile::DP3));
			req.gn.communication_profile = geonet::CommunicationProfile::ITS_G5;
			req.gn.its_aid = example_its_aid;

			// CAVISE
			///////////////////////////////////////////
			std::string OpenCDA_filename = "Messages/OpenCDA_message.proto";

			// Создаем новый объект сообщения, полученного от OpenCDA
			structure_opencda::OpenCDA_message opencda_message;
			structure_artery::Artery_message artery_message;
			// Читаем сериализованные данные из файла
			std::ifstream file(OpenCDA_filename, std::ios::in | std::ios::binary);
			// Проверяем целостность файла
			if (!file) {
				EV_INFO << "Failed to open " << OpenCDA_filename << std::endl;
				throw cRuntimeError("Error while reading file");
			}
			if (!opencda_message.ParseFromIstream(&file)) {
				EV_INFO << "Failed to parse " << OpenCDA_filename << std::endl;
				throw cRuntimeError("Error while parsing file");
			}
			file.close();


			// Выводим информацию
			std::ofstream outputFile("Logs/Received_message_logs.log", std::ios::app);
			// Проход по всем Cav внутри OpenCDA_message и вывод информации о них
			int k = 0;
			for (const auto& cav : opencda_message.cav()) {
				outputFile << "Cav vid: " << cav.vid() << std::endl;
				EV_INFO << "Cav vid: " << cav.vid() << std::endl;
				k++;
				// Добавьте другие поля Cav, которые вы хотите вывести
			}
			outputFile << "Cav k: " << k << std::endl;
			EV_INFO << "Cav k: " << k << std::endl;

			std::string serialized_data;
			if (!opencda_message.SerializeToString(&serialized_data)) {
				EV_INFO << "Failed to serialize cav_message" << std::endl;
				throw cRuntimeError("Error while serializing file");
			}
			// cPacket *packet = new cPacket(serialized_data.c_str());
			cPacket *packet = new cPacket("OpenCDA_message");
            packet->setByteLength(serialized_data.size());
            packet->setKind(0); // Set kind as required
            packet->encapsulate(new cPacket(serialized_data.c_str())); 
			// Отправляем пакет через определенный сетевой интерфейс
			request(req, packet, network.get());
		} else {
			EV_ERROR << "No network interface available for channel " << channel << "\n";
		}
	}
}

void ArteryManager::receiveSignal(cComponent* source, simsignal_t signal, cObject*, cObject*)
{
	Enter_Method("receiveSignal");

	if (signal == scSignalCamReceived) {
		auto& vehicle = getFacilities().get_const<traci::VehicleController>();
		EV_INFO << "Vehicle " << vehicle.getVehicleId() << " received a CAM in sibling serivce\n";
	}
}

}