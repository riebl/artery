#ifndef MOBILITYBASE_H_1SQMAVHF
#define MOBILITYBASE_H_1SQMAVHF

#include "artery/traci/ControllableVehicle.h"
#include "traci/LiteAPI.h"
#include "traci/VehicleSink.h"
#include <memory>
#include <string>


class MobilityBase :
    public traci::VehicleSink, // for receiving updates from TraCI
    public ControllableVehicle // for controlling the vehicle via TraCI
{
public:
    // traci::VehicleSink interface
    void initializeVehicle(traci::LiteAPI*, const std::string& id, const traci::TraCIBoundary&) override;
    void updateVehicle(const traci::TraCIPosition&, traci::TraCIAngle, double speed) override;

    // ControllableVehicle
    traci::VehicleController* getVehicleController() override;

protected:
    std::string mVehicleId;
    traci::LiteAPI* mTraci;
    traci::TraCIBoundary mNetBoundary;

private:
    virtual void update(const Position&, Angle, double speed) = 0;

    std::unique_ptr<traci::VehicleController> mController;
};

#endif /* MOBILITYBASE_H_1SQMAVHF */
