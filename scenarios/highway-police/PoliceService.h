#ifndef POLICESERVICE_H_
#define POLICESERVICE_H_

#include "artery/application/ItsG5Service.h"

class PoliceService : public ItsG5Service
{
    public:
        void trigger() override;

    protected:
        void initialize() override;
        const traci::VehicleController* mVehicleController = nullptr;
};

#endif /* POLICESERVICE_H_ */
