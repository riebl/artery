#ifndef CLEARLANESERVICE_H_
#define CLEARLANESERVICE_H_

#include "artery/application/ItsG5Service.h"

// forward declaration
namespace traci { class VehicleController; }

class ClearLaneService : public artery::ItsG5Service
{
    protected:
        void indicate(const vanetza::btp::DataIndication&, omnetpp::cPacket*) override;
        void initialize() override;

    private:
        void slowDown();

        traci::VehicleController* mVehicleController = nullptr;
};

#endif /* CLEARLANESERVICE_H_ */
