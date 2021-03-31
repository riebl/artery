/*
 * Artery V2X Simulation Framework
 * Copyright 2018 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "traci/VariableCache.h"

namespace traci
{

VariableCache::VariableCache(std::shared_ptr<API> api, int command, const std::string& id) :
    TraCIScopeWrapper(*api, command, 0, 0, 0), m_api(api), m_id(id)
{
}

void VariableCache::reset(const libsumo::TraCIResults& values)
{
    m_values = values;
}

SimulationCache::SimulationCache(std::shared_ptr<API> api) :
    VariableCache(api, libsumo::CMD_GET_SIM_VARIABLE, "")
{
}

PersonCache::PersonCache(std::shared_ptr<API> api, const std::string& personID) :
    VariableCache(api, libsumo::CMD_GET_PERSON_VARIABLE, personID)
{
}

VehicleCache::VehicleCache(std::shared_ptr<API> api, const std::string& vehicleID) :
    VariableCache(api, libsumo::CMD_GET_VEHICLE_VARIABLE, vehicleID)
{
}

template<>
double VariableCache::retrieve<double>(int var)
{
    return TraCIScopeWrapper::getDouble(var, m_id);
}

template<>
libsumo::TraCIPosition VariableCache::retrieve<libsumo::TraCIPosition>(int var)
{
    return TraCIScopeWrapper::getPos(var, m_id);
}

template<>
std::string VariableCache::retrieve<std::string>(int var)
{
    return TraCIScopeWrapper::getString(var, m_id);
}

template<>
std::vector<std::string> VariableCache::retrieve<std::vector<std::string>>(int var)
{
    return TraCIScopeWrapper::getStringVector(var, m_id);
}

template<>
int VariableCache::retrieve<int>(int var)
{
    return TraCIScopeWrapper::getInt(var, m_id);
}

} // namespace traci
