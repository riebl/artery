#include "traci/LiteAPI.h"

namespace traci
{

double LiteAPI::getDouble(int cmd, int var, const std::string& id)
{
    return m_api.getDouble(cmd, var, id);
}

libsumo::TraCIPositionVector LiteAPI::getPolygon(int cmd, int var, const std::string& id)
{
    return m_api.getPolygon(cmd, var, id);
}

libsumo::TraCIPosition LiteAPI::getPosition(int cmd, int var, const std::string& id)
{
    return m_api.getPosition(cmd, var, id);
}

libsumo::TraCIPosition LiteAPI::getPosition3D(int cmd, int var, const std::string& id)
{
    return m_api.getPosition3D(cmd, var, id);
}

std::string LiteAPI::getString(int cmd, int var, const std::string& id)
{
    return m_api.getString(cmd, var, id);
}

std::vector<std::string> LiteAPI::getStringVector(int cmd, int var, const std::string& id)
{
    return m_api.getStringVector(cmd, var, id);
}

libsumo::TraCIColor LiteAPI::getColor(int cmd, int var, const std::string& id)
{
    return m_api.getColor(cmd, var, id);
}

int LiteAPI::getInt(int cmd, int var, const std::string& id)
{
    return m_api.getInt(cmd, var, id);
}


} // namespace traci
