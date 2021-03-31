/*
 * Artery V2X Simulation Framework
 * Copyright 2018 Bernd Lehmann, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "traci/API.h"
#include "traci/BasicSubscriptionManager.h"
#include "traci/CheckTimeSync.h"
#include "traci/Core.h"
#include "traci/VariableCache.h"
#include <inet/common/ModuleAccess.h>
#include <algorithm>

using namespace omnetpp;

namespace traci
{

Define_Module(BasicSubscriptionManager)

BasicSubscriptionManager::BasicSubscriptionManager() : m_api(nullptr)
{
}

void BasicSubscriptionManager::initialize()
{
    Core* core = inet::getModuleFromPar<Core>(par("coreModule"), this);
    subscribeTraCI(core);
    m_api = core->getAPI();
    m_sim_cache = std::make_shared<SimulationCache>(m_api);
    m_ignore_persons = par("ignorePersons");
}

void BasicSubscriptionManager::finish()
{
    m_api = nullptr;
    unsubscribeTraCI();
    cSimpleModule::finish();
}

void BasicSubscriptionManager::traciInit()
{
    using namespace libsumo;
    static const std::set<int> vars {
        VAR_DEPARTED_VEHICLES_IDS,
        VAR_ARRIVED_VEHICLES_IDS,
        VAR_DELTA_T,
        VAR_TELEPORT_STARTING_VEHICLES_IDS,
        VAR_TIME
    };
    subscribeSimulationVariables(vars);

    // subscribe already running vehicles
    for (const std::string& id : m_api->vehicle.getIDList()) {
        subscribeVehicle(id);
    }

    // subscribe already running persons
    for (const std::string& id : m_api->person.getIDList()) {
        subscribePerson(id);
    }

    // read SUMO start time and store it as offset
    m_offset = omnetpp::SimTime { m_api->simulation.getCurrentTime(), omnetpp::SIMTIME_MS };
    m_offset -= omnetpp::simTime();
}

void BasicSubscriptionManager::traciStep()
{
}

void BasicSubscriptionManager::traciClose()
{
}

void BasicSubscriptionManager::subscribePerson(const std::string& id)
{
    if (!m_person_vars.empty()) {
        updatePersonSubscription(id, m_person_vars);
    }
    m_subscribed_persons.insert(id);
}

void BasicSubscriptionManager::unsubscribePerson(const std::string& id, bool person_exists)
{
    if (person_exists && !m_person_vars.empty()) {
        static const std::vector<int> empty;
        updatePersonSubscription(id, empty);
    }
    m_subscribed_persons.insert(id);
}

void BasicSubscriptionManager::updatePersonSubscription(const std::string& id, const std::vector<int>& vars)
{
    m_api->person.subscribe(id, vars, libsumo::INVALID_DOUBLE_VALUE, libsumo::INVALID_DOUBLE_VALUE);
}

void BasicSubscriptionManager::subscribePersonVariables(const std::set<int>& add_vars)
{
    std::vector<int> tmp_vars;
    std::set_union(m_person_vars.begin(), m_person_vars.end(), add_vars.begin(), add_vars.end(), std::back_inserter(tmp_vars));
    std::swap(m_person_vars, tmp_vars);
    ASSERT(m_person_vars.size() >= tmp_vars.size());

    if (m_person_vars.size() != tmp_vars.size()) {
        for (const std::string& vehicle : m_subscribed_persons) {
            updatePersonSubscription(vehicle, m_person_vars);
        }
    }
}

void BasicSubscriptionManager::subscribeVehicle(const std::string& id)
{
    if (!m_vehicle_vars.empty()) {
        updateVehicleSubscription(id, m_vehicle_vars);
    }
    m_subscribed_vehicles.insert(id);
}

void BasicSubscriptionManager::unsubscribeVehicle(const std::string& id, bool vehicle_exists)
{
    if (vehicle_exists && !m_vehicle_vars.empty()) {
        static const std::vector<int> empty;
        updateVehicleSubscription(id, empty);
    }
    m_subscribed_vehicles.erase(id);
}

void BasicSubscriptionManager::updateVehicleSubscription(const std::string& id, const std::vector<int>& vars)
{
    m_api->vehicle.subscribe(id, vars, libsumo::INVALID_DOUBLE_VALUE, libsumo::INVALID_DOUBLE_VALUE);
}

void BasicSubscriptionManager::subscribeVehicleVariables(const std::set<int>& add_vars)
{
    std::vector<int> tmp_vars;
    std::set_union(m_vehicle_vars.begin(), m_vehicle_vars.end(), add_vars.begin(), add_vars.end(), std::back_inserter(tmp_vars));
    std::swap(m_vehicle_vars, tmp_vars);
    ASSERT(m_vehicle_vars.size() >= tmp_vars.size());

    if (m_vehicle_vars.size() != tmp_vars.size()) {
        for (const std::string& vehicle : m_subscribed_vehicles) {
            updateVehicleSubscription(vehicle, m_vehicle_vars);
        }
    }
}

void BasicSubscriptionManager::subscribeSimulationVariables(const std::set<int>& add_vars)
{
    std::vector<int> tmp_vars;
    std::set_union(m_sim_vars.begin(), m_sim_vars.end(), add_vars.begin(), add_vars.end(), std::back_inserter(tmp_vars));
    std::swap(m_sim_vars, tmp_vars);
    ASSERT(m_sim_vars.size() >= tmp_vars.size());

    if (m_sim_vars.size() != tmp_vars.size()) {
        m_api->simulation.subscribe("", m_sim_vars, libsumo::INVALID_DOUBLE_VALUE, libsumo::INVALID_DOUBLE_VALUE);
    }
}

void BasicSubscriptionManager::step()
{
    const auto& simvars = m_api->simulation.getSubscriptionResults("");
    m_sim_cache->reset(simvars);
    ASSERT(checkTimeSync(*m_sim_cache, omnetpp::simTime() + m_offset));

    const auto& arrivedVehicles = m_sim_cache->get<libsumo::VAR_ARRIVED_VEHICLES_IDS>();
    for (const auto& id : arrivedVehicles) {
        unsubscribeVehicle(id, false);
    }

    const auto& departedVehicles = m_sim_cache->get<libsumo::VAR_DEPARTED_VEHICLES_IDS>();
    for (const auto& id : departedVehicles) {
        subscribeVehicle(id);
    }

    const auto& vehicles = m_api->vehicle;
    for (const std::string& vehicle : m_subscribed_vehicles) {
        const auto& vars = vehicles.getSubscriptionResults(vehicle);
        getVehicleCache(vehicle)->reset(vars);
    }

    if (!m_ignore_persons) {
        const auto& arrivedPersons = m_sim_cache->get<libsumo::VAR_ARRIVED_PERSONS_IDS>();
        for (const auto& id : arrivedPersons) {
            unsubscribePerson(id, false);
        }

        const auto& departedPersons = m_sim_cache->get<libsumo::VAR_DEPARTED_PERSONS_IDS>();
        for (const auto& id : departedPersons) {
            subscribePerson(id);
        }

        const auto& persons = m_api->person;
        for (const std::string& person : m_subscribed_persons) {
            const auto& vars = persons.getSubscriptionResults(person);
            getPersonCache(person)->reset(vars);
        }
    }
}

std::shared_ptr<PersonCache> BasicSubscriptionManager::getPersonCache(const std::string& id)
{
    auto found = m_person_caches.find(id);
    if (found == m_person_caches.end()) {
        std::tie(found, std::ignore) = m_person_caches.emplace(id, std::make_shared<PersonCache>(m_api, id));
    }
    return found->second;
}

std::shared_ptr<VehicleCache> BasicSubscriptionManager::getVehicleCache(const std::string& id)
{
    auto found = m_vehicle_caches.find(id);
    if (found == m_vehicle_caches.end()) {
        std::tie(found, std::ignore) = m_vehicle_caches.emplace(id, std::make_shared<VehicleCache>(m_api, id));
    }
    return found->second;
}

std::shared_ptr<SimulationCache> BasicSubscriptionManager::getSimulationCache()
{
    ASSERT(m_sim_cache);
    return m_sim_cache;
}

const std::unordered_set<std::string>& BasicSubscriptionManager::getSubscribedPersons() const
{
    return m_subscribed_persons;
}

const std::unordered_set<std::string>& BasicSubscriptionManager::getSubscribedVehicles() const
{
    return m_subscribed_vehicles;
}

const std::unordered_map<std::string, std::shared_ptr<VehicleCache>>& BasicSubscriptionManager::getAllVehicleCaches() const
{
    return m_vehicle_caches;
}

} // namespace traci
