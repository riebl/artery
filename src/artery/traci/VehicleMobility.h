#ifndef ARTERY_VEHICLEMOBILITY_H_CITRO6M1
#define ARTERY_VEHICLEMOBILITY_H_CITRO6M1

#include "artery/traci/ControllableVehicle.h"
#include "artery/traci/MobilityBase.h"
#include "traci/VehicleSink.h"
#include "traci/VariableCache.h"
#include <string>

namespace artery
{

class VehicleMobility :
    public virtual MobilityBase,
    public traci::VehicleSink, // for receiving updates from TraCI
    public ControllableVehicle // for controlling the vehicle via TraCI
{
public:
    // traci::VehicleSink interface
    void initializeSink(std::shared_ptr<traci::API>, std::shared_ptr<traci::VehicleCache>, const traci::Boundary&) override;
    void initializeVehicle(const traci::TraCIPosition&, traci::TraCIAngle, double speed) override;
    void updateVehicle(const traci::TraCIPosition&, traci::TraCIAngle, double speed) override;

    // ControllableVehicle
    traci::VehicleController* getVehicleController() override;

protected:
    std::string mVehicleId;
    std::unique_ptr<traci::VehicleController> mController;
};

} // namespace artery

#endif /* ARTERY_VEHICLEMOBILITY_H_CITRO6M1 */
