#ifndef LITEAPI_H_BK0NSQKL
#define LITEAPI_H_BK0NSQKL

#include "traci/API.h"

namespace traci
{

class LiteAPI
{
public:
    LiteAPI(API& api) : m_api(api) {}

    API::EdgeScope& edge() { return m_api.edge; }
    API::GUIScope& gui() { return m_api.gui; }
    API::InductionLoopScope& inductionloop() { return m_api.inductionloop; }
    API::JunctionScope& junction() { return m_api.junction; }
    API::LaneScope& lane() { return m_api.lane; }
    API::MeMeScope& multientryexit() { return m_api.multientryexit; }
    API::POIScope& poi() { return m_api.poi; }
    API::PolygonScope& polygon() { return m_api.polygon; }
    API::RouteScope& route() { return m_api.route; }
    API::SimulationScope& simulation() { return m_api.simulation; }
    API::TrafficLightScope& trafficlights() { return m_api.trafficlights; }
    API::VehicleTypeScope& vehicletype() { return m_api.vehicletype; }
    API::VehicleScope& vehicle() { return m_api.vehicle; }
    API::PersonScope& person() { return m_api.person; }

private:
    API& m_api;
};

} // namespace traci

#endif /* LITEAPI_H_BK0NSQKL */

