/*
 * Artery V2X Simulation Framework
 * Copyright 2019 Alexander Willecke, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "traci/RegionOfInterestNodeManager.h"

#include <omnetpp/cxmlelement.h>

using namespace omnetpp;

namespace traci
{

Define_Module(RegionOfInterestNodeManager)


void RegionOfInterestNodeManager::initialize()
{
    BasicNodeManager::initialize();

    cXMLElement* regions = par("regionsOfInterest").xmlValue();
    for (cXMLElement* region : regions->getChildren()) {
        if (!strcmp(region->getTagName(), "polygon")) {
            /* create new polygon */
            model::polygon<model::d2::point_xy<double>> poly;

            for (cXMLElement* point : region->getChildren()) {
                if (!strcmp(point->getTagName(),"point")) {
                    /* iterate points in polygon */
                    double x = boost::lexical_cast<double>(point->getAttribute("x"));
                    double y = boost::lexical_cast<double>(point->getAttribute("y"));

                    append(poly, boost::make_tuple(x, y));
                }
            }
            /* append polygon list of polygons */
            m_regions.push_back(model::polygon<model::d2::point_xy<double>> (poly));
        }
    }
}

void RegionOfInterestNodeManager::traciInit()
{
    BasicNodeManager::traciInit();

    /* validate regions */
    Boundary scenario_boundary = Boundary { BasicNodeManager::getLiteAPI()->simulation().getNetBoundary() };

    model::polygon<model::d2::point_xy<double>> boundary;
    append(boundary, boost::make_tuple(scenario_boundary.lowerLeftPosition().x, scenario_boundary.lowerLeftPosition().y));
    append(boundary, boost::make_tuple(scenario_boundary.lowerLeftPosition().x, scenario_boundary.upperRightPosition().y));
    append(boundary, boost::make_tuple(scenario_boundary.upperRightPosition().x, scenario_boundary.upperRightPosition().y));
    append(boundary, boost::make_tuple(scenario_boundary.upperRightPosition().x, scenario_boundary.lowerLeftPosition().y));

    for (auto &region: m_regions) {
        if (!within(region, boundary)) {
            EV_WARN << "Regions are out of scenario boundary!" << endl;
        }
    }
    EV_INFO << "Added " << m_regions.size() << " Regions of Interest to simulation" << endl;
}


void RegionOfInterestNodeManager::traciStep()
{
    processVehiclesFromSimCache();
    checkRoI();
    emit(updateNodeSignal, getNumberOfNodes());
}



void RegionOfInterestNodeManager::addVehicle(const std::string& id)
{
    if (m_regions.size() == 0) {
        BasicNodeManager::addVehicle(id);
        return;
    }

    auto vehicle = BasicNodeManager::getSubscriptions()->getVehicleCache(id);

    /* get vehicle position */
    const TraCIPosition& tmp = vehicle->get<libsumo::VAR_POSITION>();
    boost::tuple<double, double> vehicle_pos = boost::make_tuple(tmp.x,tmp.y);

    for (auto &region: m_regions) {
        /* check if vehicle is in Region of Interest */
        if (within(vehicle_pos, region)) {
            /* vehicle was in region and NOT in vehicle list */
            EV_DEBUG << "Vehicle " << id << " is added: departed within a region" << endl;
            BasicNodeManager::addVehicle(id);
            return;
        }
    }
    EV_DEBUG << "Vehicle " << id << " departed: departed outside a region of interest" << endl;
}

void RegionOfInterestNodeManager::removeVehicle(const std::string& id)
{
    if (m_regions.size() == 0) {
        BasicNodeManager::removeVehicle(id);
        return;
    }

    auto vehicle = BasicNodeManager::getSubscriptions()->getVehicleCache(id);

    /* get vehicle position */
    const TraCIPosition& tmp = vehicle->get<libsumo::VAR_POSITION>();
    boost::tuple<double, double> vehicle_pos = boost::make_tuple(tmp.x,tmp.y);

    for (auto region: m_regions) {
        /* check if vehicle is in RoI */
        if (within(vehicle_pos, region)) {
            /* vehicle was in region and in vehicle list */
            if (getVehicleMap().find(id) != getVehicleMap().end()) {
                EV_DEBUG << "Vehicle " << id << " is removed: arrived inside RoI" << endl;
                BasicNodeManager::removeVehicle(id);
            }
        }
    }
}

void RegionOfInterestNodeManager::updateVehicle(const std::string& id, VehicleSink* sink)
{
    if (m_regions.size() == 0) {
        BasicNodeManager::updateVehicle(id, sink);
        return;
    }

    auto vehicle = BasicNodeManager::getSubscriptions()->getVehicleCache(id);

    /* get vehicle position */
    const TraCIPosition& tmp = vehicle->get<libsumo::VAR_POSITION>();
    boost::tuple<double, double> vehicle_pos = boost::make_tuple(tmp.x,tmp.y);

    for (auto region: m_regions) {
        /* check if vehicle is in Region of Interest */
        if (within(vehicle_pos, region)) {
            /* vehicle is known and in RoI */
            BasicNodeManager::updateVehicle(id, sink);
            return;
        }
    }

    /* known vehicle left Region of Interest */
    EV_DEBUG << "Vehicle " << id << " was removed: left RoI" << endl;
    BasicNodeManager::removeVehicle(id);
}

void RegionOfInterestNodeManager::checkRoI()
{
    if (m_regions.size() > 0) {
        auto &current_vehicles = BasicNodeManager::getSubscriptions()->getSubscribedVehicles();

        for (const auto& id : current_vehicles) {
            /* iterate all known vehicles */
            if (getVehicleMap().find(id) == getVehicleMap().end()) {
                /* vehicle is not managed */
                auto vehicle = BasicNodeManager::getSubscriptions()->getVehicleCache(id);

                /* get vehicle position */
                const TraCIPosition& tmp = vehicle->get<libsumo::VAR_POSITION>();
                boost::tuple<double, double> vehicle_pos = boost::make_tuple(tmp.x,tmp.y);

                for (auto region: m_regions) {
                    /* check if vehicle is in Region of Interest */
                    if (within(vehicle_pos, region)) {
                        EV_DEBUG << "Vehicle " << id << " is added: entered RoI" << endl;
                        BasicNodeManager::addVehicle(id);
                        return;
                    }
                }
            }
        }
    }
}
} // namespace traci
