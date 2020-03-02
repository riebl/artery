/*
 * Artery V2X Simulation Framework
 * Copyright 2020 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef TRACI_VEHICLELIFECYCLE_H_9CW2JCKB
#define TRACI_VEHICLELIFECYCLE_H_9CW2JCKB

#include <string>

namespace traci
{

/**
 * VehicleLifecycle is an interface employed by VehiclePolicy.
 * It enables policies to trigger actions (add, update and remove) unsolicitedly,
 * e.g. after expiry of a local timer or a condition.
 */
class VehicleLifecycle
{
public:
    virtual void addVehicle(const std::string& id) = 0;
    virtual void updateVehicle(const std::string& id) = 0;
    virtual void removeVehicle(const std::string& id) = 0;

    virtual ~VehicleLifecycle() = default;
};

} // namespace traci

#endif /* TRACI_VEHICLELIFECYCLE_H_9CW2JCKB */

