/*
 * Artery V2X Simulation Framework
 * Copyright 2020 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef TRACI_EXTENSIBLENODEMANAGER_H_7AC9LLFH
#define TRACI_EXTENSIBLENODEMANAGER_H_7AC9LLFH

#include "traci/BasicNodeManager.h"
#include "traci/VehicleLifecycle.h"
#include "traci/VehiclePolicy.h"
#include <list>
#include <vector>

namespace traci
{

/**
 * This node manager is extensible via configurable VehiclePolicy sub-modules.
 *
 * Each VehiclePolicy is invoked on vehicle events (add, update or remove) and
 * can decide on further processing of the particular vehicle. If no policy
 * does actively discard an event, the default action will be performed after
 * interrogation of all policies.
 */
class ExtensibleNodeManager : public BasicNodeManager
{
protected:
    void initialize() override;
    void finish() override;

    void traciInit() override;

    void processVehicles() override;
    void addVehicle(const std::string&) override;
    void updateVehicle(const std::string&, VehicleSink*) override;
    void removeVehicle(const std::string&) override;

    friend class VehicleLifecycle;
    void addVehicle(const VehiclePolicy* omit, const std::string&);
    void removeVehicle(const VehiclePolicy* omit, const std::string&);
    void updateVehicle(const VehiclePolicy* omit, const std::string&, VehicleSink*);

private:
    class VehicleLifecycle : public traci::VehicleLifecycle
    {
    public:
        VehicleLifecycle(ExtensibleNodeManager* manager, const VehiclePolicy* ext);
        void addVehicle(const std::string& id) override;
        void removeVehicle(const std::string& id) override;
        void updateVehicle(const std::string& id) override;

    private:
        ExtensibleNodeManager* m_manager;
        const VehiclePolicy* m_policy;
    };

    std::vector<VehiclePolicy*> m_policies;
    std::list<VehicleLifecycle> m_lifecycles;
    std::vector<std::string> m_remove_vehicles;
};

} // namespace traci

#endif /* TRACI_EXTENSIBLENODEMANAGER_H_7AC9LLFH */

