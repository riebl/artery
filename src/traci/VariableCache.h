/*
 * Artery V2X Simulation Framework
 * Copyright 2018 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef VARIABLECACHE_H_GJG2APIF
#define VARIABLECACHE_H_GJG2APIF

#include "traci/API.h"
#include "traci/ValueUtils.h"
#include "traci/VariableTraits.h"
#include <memory>
#include <string>

namespace traci
{

class VariableCache : private TraCIAPI::TraCIScopeWrapper
{
public:
    const std::string& getId() const { return m_id; }

    /**
     * Get value from cache as shared pointer.
     * If variable is not present yet, it is automatically retrieved.
     *
     * \param VAR variable identifier
     * \return read-only shared pointer to TraCI result type
     */
    template<int VAR>
    std::shared_ptr<const typename VariableTrait<VAR>::result_type> getPtr()
    {
        using value_type = typename VariableTrait<VAR>::value_type;
        using result_type = typename VariableTrait<VAR>::result_type;

        auto found = m_values.find(VAR);
        if (found == m_values.end()) {
            value_type value = retrieve<value_type>(VAR);
            auto result = std::make_shared<result_type>(make_value(std::move(value)));
            std::tie(found, std::ignore) = m_values.emplace(VAR, std::move(result));
        }

        return std::dynamic_pointer_cast<result_type>(found->second);
    }

    /**
     * Get value from cache.
     * If variable is not present yet, it is automatically retrieved.
     * Result type is unwrapped as far as possible, e.g. TraCIInt -> int
     *
     * \param VAR variable identifier
     * \return cached value (cache cannot be modified through returned type)
     */
    template<int VAR>
    auto get() ->
    typename get_value_trait<typename VariableTrait<VAR>::value_type>::return_type
    {
        using value_type = typename VariableTrait<VAR>::value_type;
        return get_value<value_type>(this->getPtr<VAR>());
    }

    /**
     * Reset cache, i.e all previously stored values are dropped
     * \param values new values to be stored
     */
    void reset(const libsumo::TraCIResults& values);

protected:
    VariableCache(std::shared_ptr<API> api, int command, const std::string& id);

    template<typename T>
    T retrieve(int var);

private:
    std::shared_ptr<API> m_api;
    const std::string m_id;
    libsumo::TraCIResults m_values;
};

class PersonCache : public VariableCache
{
public:
    PersonCache(std::shared_ptr<API> api, const std::string& personID);
    const std::string& getPersonId() const { return getId(); }
};

class VehicleCache : public VariableCache
{
public:
    VehicleCache(std::shared_ptr<API> api, const std::string& vehicleID);
    const std::string& getVehicleId() const { return getId(); }
};

class SimulationCache : public VariableCache
{
public:
    SimulationCache(std::shared_ptr<API> api);
};

template<>
double VariableCache::retrieve<double>(int var);

template<>
libsumo::TraCIPosition VariableCache::retrieve<libsumo::TraCIPosition>(int var);

template<>
std::string VariableCache::retrieve<std::string>(int var);

template<>
std::vector<std::string> VariableCache::retrieve<std::vector<std::string>>(int var);

template<>
int VariableCache::retrieve<int>(int var);

} // namespace traci

#endif /* VARIABLECACHE_H_GJG2APIF */

