#ifndef VEHICLE_H_
#define VEHICLE_H_

#include "artery/traci/VehicleController.h"
#include "artery/application/VehicleDataProvider.h"
#include <map>

class VehicleDataProvider;

namespace Veins {
    class TraCIMobility;
}

/**
 * Struct containing all necessary vehicle data
 */
struct Vehicle
{
    Vehicle(traci::VehicleController& ctrl, const VehicleDataProvider& vehicleDataProvider, std::map<std::string, Vehicle> & v) :
        controller(ctrl), vdp(vehicleDataProvider), vehicles(v) {}

    traci::VehicleController& controller;
    const VehicleDataProvider& vdp;
    std::map<std::string, Vehicle>& vehicles;
};

#endif /* VEHICLE_H_ */
