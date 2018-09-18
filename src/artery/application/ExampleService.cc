//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "ExampleService.h"
#include "artery/traci/VehicleController.h"
#include <omnetpp/cpacket.h>
#include <vanetza/btp/data_request.hpp>
#include <vanetza/dcc/profile.hpp>
#include <vanetza/geonet/interface.hpp>

using namespace omnetpp;
using namespace vanetza;

static const simsignal_t scSignalCamReceived = cComponent::registerSignal("CamReceived");

Define_Module(ExampleService)

ExampleService::ExampleService()
{
}

ExampleService::~ExampleService()
{
	cancelAndDelete(m_self_msg);
}

void ExampleService::indicate(const btp::DataIndication& ind, cPacket* packet)
{
	if (packet->getByteLength() == 42) {
		EV_INFO << "packet indication\n";
	}

	delete(packet);
}

void ExampleService::initialize()
{	
    static int num = 0 ; 
	printf("INITIALIZE OWN Service %d \n", num++);
    
    ItsG5Service::initialize();
	m_self_msg = new cMessage("Example Service");
	subscribe(scSignalCamReceived);

	scheduleAt(simTime() + 3.0, m_self_msg);
}

void ExampleService::finish()
{
	// you could record some scalars at this point
	ItsG5Service::finish();
}       

void ExampleService::handleMessage(cMessage* msg)
{
	Enter_Method("handleMessage");
	if (msg == m_self_msg) {
		EV_INFO << "self message\n";
	}
}

void ExampleService::trigger()
{
	Enter_Method("trigger");
	btp::DataRequestB req;
	req.destination_port = host_cast<ExampleService::port_type>(getPortNumber());
	req.gn.transport_type = geonet::TransportType::SHB;
	req.gn.traffic_class.tc_id(static_cast<unsigned>(dcc::Profile::DP3));
	req.gn.communication_profile = geonet::CommunicationProfile::ITS_G5;

	cPacket* packet = new cPacket("Example Service Packet");
	packet->setByteLength(42);
	request(req, packet);
}

void ExampleService::receiveSignal(cComponent* source, simsignal_t signal, cObject* obj, cObject* details)
{
	if (signal == scSignalCamReceived) {
		auto& vehicle = getFacilities().get_const<traci::VehicleController>();
        EV_INFO << "Vehicle " << vehicle.getVehicleId() << " received a CAM in sibling serivce\n";
        
        // dynamic_cast <new_type> (expression)
        const vanetza::asn1::Cam& msg =  dynamic_cast <CaObject*>(obj)->asn1();
        std::cout << "Vehicle " << vehicle.getVehicleId() << " "
//         << source->getNumParams()  // 7
//         << obj->getClassName()<< " " // CaObject  
//         << obj->getName()<< " " // NONE
//         << obj->info()<< " " // NONE
//         << obj->str()<< " " // NONE  
        << msg->cam.camParameters.basicContainer.stationType<< " " // 5  
        << msg->cam.camParameters.basicContainer.referencePosition.longitude<< " " // 5  
        << msg->cam.camParameters.highFrequencyContainer.choice.basicVehicleContainerHighFrequency.longitudinalAcceleration.longitudinalAccelerationValue<< " " // 5  
        << " received a CAM in sibling serivce\n";
        
        
        
        
    }
}





