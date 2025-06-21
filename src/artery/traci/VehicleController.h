#ifndef VEHICLECONTROLLER_H_AXBS5NQM
#define VEHICLECONTROLLER_H_AXBS5NQM

#include "artery/traci/Controller.h"
#include "artery/traci/VehicleType.h"

namespace traci
{

class VehicleCache;

class VehicleController : public Controller
{
public:
    VehicleController(std::shared_ptr<traci::API>, const std::string& id);
    VehicleController(std::shared_ptr<traci::API>, std::shared_ptr<VehicleCache> cache);

    const std::string& getVehicleId() const;
    const VehicleType& getVehicleType() const;
    const std::string getVehicleClass() const;

    Velocity getMaxSpeed() const;
    void setMaxSpeed(Velocity);
    void setSpeed(Velocity);
    void setSpeedFactor(double);

    void changeTarget(const std::string& edge);

private:
    VehicleType m_type;
};

} // namespace traci

#endif /* VEHICLECONTROLLER_H_AXBS5NQM */

