/*
 * Artery V2X Simulation Framework
 * Copyright 2018 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef VARIABLECACHE_H_GJG2APIF
#define VARIABLECACHE_H_GJG2APIF

#include "traci/LiteAPI.h"
#include "traci/ValueUtils.h"
#include "traci/VariableTraits.h"
#include <string>

namespace traci
{

class VariableCache
{
public:
    LiteAPI& getLiteAPI() { return m_api; }
    const std::string& getId() const { return m_id; }

    /**
     * Get value from cache.
     * If variable is not present yet, it is automatically retrieved.
     *
     * \param VAR variable identifier
     */
    template<int VAR>
    typename VariableTrait<VAR>::value_type get()
    {
        using value_type = typename VariableTrait<VAR>::value_type;

        auto found = m_values.find(VAR);
        if (found == m_values.end()) {
            value_type tmp = retrieve<value_type>(VAR);
            std::tie(found, std::ignore) =
                m_values.emplace(VAR, libsumo::make_value(std::move(tmp)));
        }

        return libsumo::value_cast<value_type>(found->second);
    }

    /**
     * Reset cache, i.e all previously stored values are dropped
     * \param values new values to be stored
     */
    void reset(const TraCIAPI::TraCIValues& values);

protected:
    VariableCache(LiteAPI& api, int command, const std::string& id);

    template<typename T>
    T retrieve(int var);

private:
    LiteAPI& m_api;
    const std::string m_id;
    const int m_command;
    TraCIAPI::TraCIValues m_values;
};

class VehicleCache : public VariableCache
{
public:
    VehicleCache(LiteAPI& api, const std::string& vehicleID);
    const std::string& getVehicleId() const { return getId(); }
};

class SimulationCache : public VariableCache
{
public:
    SimulationCache(LiteAPI& api);
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
SUMOTime VariableCache::retrieve<SUMOTime>(int var);

template<>
int VariableCache::retrieve<int>(int var);

} // namespace traci

#endif /* VARIABLECACHE_H_GJG2APIF */

