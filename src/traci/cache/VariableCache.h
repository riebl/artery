/*
 * Artery V2X Simulation Framework
 * Copyright 2018 Raphael Riebl
 * Copyright 2026 Narkhov Evgeny
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#pragma once

#include <libsumo/TraCIConstants.h>
#include <omnetpp/cexception.h>
#include <traci/API.h>
#include <traci/cache/ValueUtils.h>
#include <traci/cache/VariableTraits.h>

#include <memory>
#include <string>

namespace artery {

namespace traci {

namespace detail {

template <typename Tag, int Variable>
struct Retriever;

/****************************
 * Tags for types of caches *
 ****************************
 */

struct VariableCacheTag;
struct PersonCacheTag;
struct VehicleCacheTag;
struct SimulationCacheTag;

}  // namespace detail

template <typename Tag = detail::VariableCacheTag>
class VariableCache
{
public:
    const std::string& getId() const { return id_; }

    /**
     * @brief Retrieve target value from simulation. This is not intended to be called directly:
     * prefer using a smarter version that also uses cache - @ref getPointer or @ref get.
     */
    template <int Variable>
    auto retrieve() -> typename VariableTrait<Variable>::value_type {
        return detail::Retriever<Tag, Variable>::get(id_);
    }

    /**
     * Get value from cache as shared pointer.
     * If variable is not present yet, it is automatically retrieved.
     *
     * @tparam Variable variable identifier
     * @return read-only shared pointer to TraCI result type
     */
    template <int Variable>
    auto getPointer() -> std::shared_ptr<const typename VariableTrait<Variable>::result_type> {
        using value_type = typename VariableTrait<Variable>::value_type;
        using result_type = typename VariableTrait<Variable>::result_type;

        auto found = values_.find(Variable);
        if (found == values_.end()) {
            value_type value = retrieve<Variable>();
            auto result = std::make_shared<result_type>(make_value(std::move(value)));
            std::tie(found, std::ignore) = values_.emplace(Variable, std::move(result));
        }

        return std::dynamic_pointer_cast<result_type>(found->second);
    }

    /**
     * Get value from cache.
     * If variable is not present yet, it is automatically retrieved.
     * Result type is unwrapped as far as possible, e.g. TraCIInt -> int
     *
     * \tparam Variable variable identifier
     * \return cached value (cache cannot be modified through returned type)
     */
    template <int Variable>
    auto get() -> typename VariableTrait<Variable>::result_type {
        using value_type = typename VariableTrait<Variable>::value_type;
        return getPointer<value_type>(this->getPointer<Variable>());
    }

    /**
     * Reset cache, i.e all previously stored values are dropped
     * \param values new values to be stored
     */
    void reset(const lib::TraCIResults& values) { values_ = values; }

protected:
    VariableCache(std::shared_ptr<API> api, const std::string& id) : api_(api), id_(id) {}


private:
    const std::string id_;
    std::shared_ptr<API> api_;
    lib::TraCIResults values_;
};

class PersonCache : public VariableCache<detail::PersonCacheTag>
{
public:
    PersonCache(std::shared_ptr<API> api, const std::string& personID) : VariableCache(api, personID) {}
    const std::string& getPersonId() const { return getId(); }
};

class VehicleCache : public VariableCache<detail::VehicleCacheTag>
{
public:
    VehicleCache(std::shared_ptr<API> api, const std::string& vehicleID) : VariableCache(api, vehicleID) {}
    const std::string& getVehicleId() const { return getId(); }
};

class SimulationCache : public VariableCache<detail::SimulationCacheTag>
{
public:
    SimulationCache(std::shared_ptr<API> api) : VariableCache(api, "") {}
};

namespace detail {

template <typename Tag, int Variable>
struct Retriever {
    static typename VariableTrait<Variable>::value_type get(const std::string& /* id */) {
        throw omnetpp::cRuntimeError("retrieval is not implemented for variable with integer code %d", Variable);
    }
};

template <>
struct Retriever<SimulationCacheTag, lib::VAR_TIME> {
    static typename VariableTrait<lib::VAR_TIME>::value_type get(const std::string& /* id */) { return lib::Simulation::getTime(); }
};

template <>
struct Retriever<SimulationCacheTag, lib::VAR_DELTA_T> {
    static typename VariableTrait<lib::VAR_DELTA_T>::value_type get(const std::string& /* id */) { return lib::Simulation::getDeltaT(); }
};

template <>
struct Retriever<SimulationCacheTag, lib::VAR_DEPARTED_VEHICLES_IDS> {
    static typename VariableTrait<lib::VAR_DEPARTED_VEHICLES_IDS>::value_type get(const std::string& /* id */) { return lib::Simulation::getDepartedIDList(); }
};

template <>
struct Retriever<SimulationCacheTag, lib::VAR_ARRIVED_VEHICLES_IDS> {
    static typename VariableTrait<lib::VAR_ARRIVED_VEHICLES_IDS>::value_type get(const std::string& /* id */) { return lib::Simulation::getArrivedIDList(); }
};

template <>
struct Retriever<SimulationCacheTag, lib::VAR_TELEPORT_STARTING_VEHICLES_IDS> {
    static typename VariableTrait<lib::VAR_TELEPORT_STARTING_VEHICLES_IDS>::value_type get(const std::string& /* id */) { return lib::Simulation::getStartingTeleportIDList(); }
};

// VAR_TIME_STEP?

template <>
struct Retriever<SimulationCacheTag, lib::VAR_ARRIVED_PERSONS_IDS> {
    static typename VariableTrait<lib::VAR_ARRIVED_PERSONS_IDS>::value_type get(const std::string& /* id */) { return lib::Simulation::getArrivedPersonIDList(); }
};

template <>
struct Retriever<SimulationCacheTag, lib::VAR_DEPARTED_PERSONS_IDS> {
    static typename VariableTrait<lib::VAR_DEPARTED_PERSONS_IDS>::value_type get(const std::string& /* id */) { return lib::Simulation::getDepartedPersonIDList(); }
};

// VAR_SIGNALS?

template <>
struct Retriever<SimulationCacheTag, lib::VAR_WIDTH> {
    static typename VariableTrait<lib::VAR_WIDTH>::value_type get(const std::string& id) { return lib::Vehicle::getWidth(id); }
};

// fill others too

}  // namespace detail

}  // namespace traci

}  // namespace artery
