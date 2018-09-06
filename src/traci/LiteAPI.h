#ifndef LITEAPI_H_BK0NSQKL
#define LITEAPI_H_BK0NSQKL

#include "traci/API.h"

namespace traci
{

class LiteAPI
{
public:
    LiteAPI(API& api) : m_api(api) {}

    TraCIGeoPosition convertGeo(const TraCIPosition& pos) const { return m_api.convertGeo(pos); }
    TraCIPosition convert2D(const TraCIGeoPosition& pos) const { return m_api.convert2D(pos); }

    API::EdgeScope& edge() { return m_api.edge; }
    const API::EdgeScope& edge() const { return m_api.edge; }
    API::GUIScope& gui() { return m_api.gui; }
    const API::GUIScope& gui() const { return m_api.gui; }
    API::InductionLoopScope& inductionloop() { return m_api.inductionloop; }
    const API::InductionLoopScope& inductionloop() const { return m_api.inductionloop; }
    API::JunctionScope& junction() { return m_api.junction; }
    const API::JunctionScope& junction() const { return m_api.junction; }
    API::LaneScope& lane() { return m_api.lane; }
    const API::LaneScope& lane() const { return m_api.lane; }
    API::MeMeScope& multientryexit() { return m_api.multientryexit; }
    const API::MeMeScope& multientryexit() const { return m_api.multientryexit; }
    API::POIScope& poi() { return m_api.poi; }
    const API::POIScope& poi() const { return m_api.poi; }
    API::PolygonScope& polygon() { return m_api.polygon; }
    const API::PolygonScope& polygon() const { return m_api.polygon; }
    API::RouteScope& route() { return m_api.route; }
    const API::RouteScope& route() const { return m_api.route; }
    API::SimulationScope& simulation() { return m_api.simulation; }
    const API::SimulationScope& simulation() const { return m_api.simulation; }
    API::TrafficLightScope& trafficlights() { return m_api.trafficlights; }
    const API::TrafficLightScope& trafficlights() const { return m_api.trafficlights; }
    API::VehicleTypeScope& vehicletype() { return m_api.vehicletype; }
    const API::VehicleTypeScope& vehicletype() const { return m_api.vehicletype; }
    API::VehicleScope& vehicle() { return m_api.vehicle; }
    const API::VehicleScope& vehicle() const { return m_api.vehicle; }
    API::PersonScope& person() { return m_api.person; }
    const API::PersonScope& person() const { return m_api.person; }

    double getDouble(int cmd, int var, const std::string& id);
    libsumo::TraCIPositionVector getPolygon(int cmd, int var, const std::string& id);
    libsumo::TraCIPosition getPosition(int cmd, int var, const std::string& id);
    libsumo::TraCIPosition getPosition3D(int cmd, int var, const std::string& id);
    std::string getString(int cmd, int var, const std::string& id);
    std::vector<std::string> getStringVector(int cmd, int var, const std::string& id);
    libsumo::TraCIColor getColor(int cmd, int var, const std::string& id);
    int getInt(int cmd, int var, const std::string& id);

private:
    API& m_api;
};

} // namespace traci

#endif /* LITEAPI_H_BK0NSQKL */

