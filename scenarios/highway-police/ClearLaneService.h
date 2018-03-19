#ifndef CLEARLANESERVICE_H_
#define CLEARLANESERVICE_H_

#include "artery/application/ItsG5Service.h"

class ClearLaneService : public ItsG5Service
{
    protected:
        void indicate(const vanetza::btp::DataIndication&, omnetpp::cPacket*) override;
        void initialize() override;

    private:
        void slowDown();

        traci::VehicleController* mVehicleController = nullptr;
};

#endif /* CLEARLANESERVICE_H_ */
