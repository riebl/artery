#ifndef VEHICLE_H_
#define VEHICLE_H_

class VehicleDataProvider;

namespace Veins {
    class TraCIMobility;
}

/**
 * Struct containing all necessary vehicle data
 */
struct Vehicle
{
    Vehicle(Veins::TraCIMobility& mob, const VehicleDataProvider& vehicleDataProvider) :
        mobility(mob), vdp(vehicleDataProvider) {}

    Veins::TraCIMobility& mobility;
    const VehicleDataProvider& vdp;
};

#endif /* VEHICLE_H_ */
