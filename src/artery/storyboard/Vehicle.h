#ifndef VEHICLE_H_
#define VEHICLE_H_

#include "artery/traci/VehicleController.h"
#include "artery/application/VehicleDataProvider.h"

/**
 * Struct containing all necessary vehicle data
 */
struct Vehicle
{
    Vehicle(traci::VehicleController& ctrl, const VehicleDataProvider& vehicleDataProvider) :
        controller(ctrl), vdp(vehicleDataProvider) {}

    traci::VehicleController& controller;
    const VehicleDataProvider& vdp;
};

#endif /* VEHICLE_H_ */
