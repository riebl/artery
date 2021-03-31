/*
 * Artery V2X Simulation Framework
 * Copyright 2018 Bernd Lehmann, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef BASICSUBSCRIPTIONMANAGER_H_BCDX4IU2
#define BASICSUBSCRIPTIONMANAGER_H_BCDX4IU2

#include "traci/Listener.h"
#include "traci/SubscriptionManager.h"
#include <omnetpp/csimplemodule.h>
#include <omnetpp/simtime.h>
#include <unordered_map>
#include <unordered_set>

namespace traci
{

class API;

class BasicSubscriptionManager : public Listener, public SubscriptionManager, public omnetpp::cSimpleModule
{
public:
    BasicSubscriptionManager();

    // implement traci::SubscriptionManager
    void step() override;
    void subscribePersonVariables(const std::set<int>& personVariables) override;
    void subscribeVehicleVariables(const std::set<int>& vehicleVariables) override;
    void subscribeSimulationVariables(const std::set<int>& simulationVariables) override;
    const std::unordered_set<std::string>& getSubscribedPersons() const override;
    const std::unordered_set<std::string>& getSubscribedVehicles() const override;
    const std::unordered_map<std::string, std::shared_ptr<VehicleCache>>& getAllVehicleCaches() const override;
    std::shared_ptr<PersonCache> getPersonCache(const std::string& id) override;
    std::shared_ptr<VehicleCache> getVehicleCache(const std::string& id) override;
    std::shared_ptr<SimulationCache> getSimulationCache() override;

protected:
    void initialize() override;
    void finish() override;

private:
    BasicSubscriptionManager(const BasicSubscriptionManager&) = delete;

    // implement traci::Listener
    void traciInit() override;
    void traciStep() override;
    void traciClose() override;

    void subscribePerson(const std::string& id);
    void unsubscribePerson(const std::string& id, bool person_exists);
    void updatePersonSubscription(const std::string& id, const std::vector<int>& vars);

    void subscribeVehicle(const std::string& id);
    void unsubscribeVehicle(const std::string& id, bool vehicle_exists);
    void updateVehicleSubscription(const std::string& id, const std::vector<int>& vars);

    std::shared_ptr<API> m_api;
    std::unordered_set<std::string> m_subscribed_persons;
    std::unordered_set<std::string> m_subscribed_vehicles;
    std::vector<int> m_person_vars;
    std::vector<int> m_vehicle_vars;
    std::vector<int> m_sim_vars;
    std::unordered_map<std::string, std::shared_ptr<PersonCache>> m_person_caches;
    std::unordered_map<std::string, std::shared_ptr<VehicleCache>> m_vehicle_caches;
    std::shared_ptr<SimulationCache> m_sim_cache;
    omnetpp::SimTime m_offset = omnetpp::SimTime::ZERO;
    bool m_ignore_persons;
};

} // namespace traci

#endif /* BASICSUBSCRIPTIONMANAGER_H_BCDX4IU2 */
