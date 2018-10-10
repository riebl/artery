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

#ifndef EXAMPLESERVICE_H_
#define EXAMPLESERVICE_H_

#include "ItsG5Service.h"

class ExampleService : public ItsG5Service
{
    public:
        ExampleService();
        ~ExampleService();

        void indicate(const vanetza::btp::DataIndication&, omnetpp::cPacket*) override;
        void trigger() override;
        void receiveSignal(cComponent*, omnetpp::simsignal_t, cObject*, cObject*) override;

    protected:
        void initialize() override;
        void finish() override;
        void handleMessage(omnetpp::cMessage*) override;

    private:
        omnetpp::cMessage* m_self_msg;
		artery::LocalDynamicMap* mLocalDynamicMap; // map of vehicles and avarness messages 
		std::string myVehicleID; 
		
};

#endif /* EXAMPLESERVICE_H_ */
