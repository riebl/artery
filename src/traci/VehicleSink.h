#ifndef VEHICLESINK_H_B1HZNP6Q
#define VEHICLESINK_H_B1HZNP6Q

#include "traci/API.h"

namespace traci
{

class LiteAPI;

class VehicleSink
{
public:
    virtual void initializeVehicle(LiteAPI*, const std::string& id, const TraCIBoundary&) = 0;
    virtual void updateVehicle(const TraCIPosition&, TraCIAngle, double speed) = 0;
    virtual ~VehicleSink() = default;
};

} // namespace traci

#endif /* VEHICLESINK_H_B1HZNP6Q */

