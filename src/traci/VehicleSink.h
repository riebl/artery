#ifndef VEHICLESINK_H_B1HZNP6Q
#define VEHICLESINK_H_B1HZNP6Q

#include "traci/API.h"
#include <memory>

namespace traci
{

class API;
class VehicleCache;

class VehicleSink
{
public:
    virtual void initializeSink(std::shared_ptr<API>, std::shared_ptr<VehicleCache>, const Boundary&) = 0;
    virtual void initializeVehicle(const TraCIPosition&, TraCIAngle, double speed) = 0;
    virtual void updateVehicle(const TraCIPosition&, TraCIAngle, double speed) = 0;
    virtual ~VehicleSink() = default;
};

} // namespace traci

#endif /* VEHICLESINK_H_B1HZNP6Q */

