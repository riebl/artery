/*
 * Artery V2X Simulation Framework
 * Copyright 2019 Alexander Willecke, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef REGIONOFINTERESTNODEMANAGER_H_AGMBROIE
#define REGIONOFINTERESTNODEMANAGER_H_AGMBROIE

#include "traci/BasicNodeManager.h"

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/adapted/boost_tuple.hpp>

#include "traci/VariableCache.h"

BOOST_GEOMETRY_REGISTER_BOOST_TUPLE_CS(cs::cartesian)

using namespace boost::geometry;

namespace traci
{

class RegionOfInterestNodeManager : public BasicNodeManager
{

protected:
    void initialize() override;

    void addVehicle(const std::string& id);
    void removeVehicle(const std::string& id);
    void updateVehicle(const std::string& id, VehicleSink* sink);
    void checkRoI();

    void traciInit() override;
    void traciStep() override;

private:
    // std::map<std::string, VehicleSink*> m_vehicles;
    std::list< model::polygon<model::d2::point_xy<double>> > m_regions;
};

} // namespace traci

#endif /* REGIONOFINTERESTNODEMANAGER_H_AGMBROIE */

