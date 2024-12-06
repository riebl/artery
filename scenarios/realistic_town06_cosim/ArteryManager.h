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

#pragma once

// STD
#include <memory>

// artery
#include <omnetpp/simtime.h>
#include <omnetpp/cmessage.h>
#include <omnetpp/csimplemodule.h>
#include <artery/application/ItsG5Service.h>
#include <artery/application/NetworkInterface.h>

// communication
#include <cavise/comms/SingletonHolder.h>
#include <cavise/comms/CommunicationManager.h>

// proto
#include <cavise/artery.pb.h>
#include <cavise/opencda.pb.h>


namespace artery {

    using CommunicationManager = cavise::CommunicationManager<structure_artery::Artery_message, structure_opencda::OpenCDA_message>;

    class ArteryManager 
        : public ItsG5Service {
    public:
        ArteryManager();
        ~ArteryManager();
        
        void trigger() override;
        void indicate(const vanetza::btp::DataIndication& ind, omnetpp::cPacket* packet, const artery::NetworkInterface& net) override;
        void receiveSignal(omnetpp::cComponent* source, omnetpp::simsignal_t signal, omnetpp::cObject* obj1, omnetpp::cObject* obj2) override;
    
    protected:
        void initialize() override;
        void finish() override;
        void handleMessage(omnetpp::cMessage*) override;

    private:
        std::shared_ptr<CommunicationManager> communicationManager_;

        omnetpp::cMessage* selfMessage_;
        omnetpp::simtime_t delta_ = 1.0;

        inline static std::atomic<bool> logInitalized_ = false;
    };

} // namespace artery
