/*
 * Artery V2X Simulation Framework
 * Copyright 2018 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "traci/VariableCache.h"

namespace traci
{

VariableCache::VariableCache(LiteAPI& api, int command, const std::string& id) :
    m_api(api), m_id(id), m_command(command)
{
}

void VariableCache::reset(const libsumo::TraCIResults& values)
{
    m_values = values;
}

SimulationCache::SimulationCache(LiteAPI& api) :
    VariableCache(api, libsumo::CMD_GET_SIM_VARIABLE, "")
{
}

VehicleCache::VehicleCache(LiteAPI& api, const std::string& vehicleID) :
    VariableCache(api, libsumo::CMD_GET_VEHICLE_VARIABLE, vehicleID)
{
}

template<>
double VariableCache::retrieve<double>(int var)
{
    return m_api.getDouble(m_command, var, m_id);
}

template<>
libsumo::TraCIPosition VariableCache::retrieve<libsumo::TraCIPosition>(int var)
{
    return m_api.getPosition(m_command, var, m_id);
}

template<>
std::string VariableCache::retrieve<std::string>(int var)
{
    return m_api.getString(m_command, var, m_id);
}

template<>
std::vector<std::string> VariableCache::retrieve<std::vector<std::string>>(int var)
{
    return m_api.getStringVector(m_command, var, m_id);
}

template<>
int VariableCache::retrieve<int>(int var)
{
    return m_api.getInt(m_command, var, m_id);
}

} // namespace traci
