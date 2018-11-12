#ifndef POLICESERVICE_H_
#define POLICESERVICE_H_

#include "artery/application/ItsG5Service.h"

// forward declaration
namespace traci { class VehicleController; }

class PoliceService : public artery::ItsG5Service
{
    public:
        void trigger() override;

    protected:
        void initialize() override;
        const traci::VehicleController* mVehicleController = nullptr;
};

#endif /* POLICESERVICE_H_ */
