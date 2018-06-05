/*
 * Artery V2X Simulation Framework
 * Copyright 2018 Bernd Lehmann, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef SUBSCRIPTIONMANAGER_H_4WCPHLVR
#define SUBSCRIPTIONMANAGER_H_4WCPHLVR

#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace traci
{

// forward declaration
class SimulationCache;
class VehicleCache;

class SubscriptionManager
{
public:
    virtual ~SubscriptionManager() = default;
    virtual void step() = 0;
    virtual void subscribeVehicleVariables(const std::set<int>& vehicleVariables) = 0;
    virtual void subscribeSimulationVariables(const std::set<int>& simulationVariables) = 0;
    virtual const std::unordered_set<std::string>& getSubscribedVehicles() const = 0;
    virtual const std::unordered_map<std::string, std::shared_ptr<VehicleCache>>& getAllVehicleCaches() const = 0;
    virtual std::shared_ptr<VehicleCache> getVehicleCache(const std::string& id) = 0;
    virtual std::shared_ptr<SimulationCache> getSimulationCache() = 0;
};

} // namespace traci

#endif /* SUBSCRIPTIONMANAGER_H_4WCPHLVR */
