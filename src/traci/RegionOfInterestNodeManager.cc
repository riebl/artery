/*
 * Artery V2X Simulation Framework
 * Copyright 2019 Alexander Willecke, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "traci/RegionOfInterestNodeManager.h"
#include "traci/LiteAPI.h"
#include "traci/VariableCache.h"

#include <omnetpp/cxmlelement.h>

using namespace omnetpp;

namespace traci
{

Define_Module(RegionOfInterestNodeManager)

void RegionOfInterestNodeManager::traciInit()
{
    BasicNodeManager::traciInit();

    /* validate regions */
    cXMLElement* regions = par("regionsOfInterest").xmlValue();
    if (regions) {
        Boundary boundary { BasicNodeManager::getLiteAPI()->simulation().getNetBoundary() };
        m_regions.initialize(*regions, boundary);
        EV_INFO << "Added " << m_regions.size() << " Regions of Interest to simulation" << endl;
    }
}

void RegionOfInterestNodeManager::processVehicles()
{
    BasicNodeManager::processVehicles();
    checkRegionOfInterest();
}

void RegionOfInterestNodeManager::addVehicle(const std::string& id)
{
    if (m_regions.empty()) {
        BasicNodeManager::addVehicle(id);
    } else {
        /* check if vehicle is in Region of Interest */
        auto vehicle = getSubscriptions()->getVehicleCache(id);
        if (m_regions.cover(vehicle->get<libsumo::VAR_POSITION>())) {
            /* vehicle was in region and NOT in vehicle list */
            EV_DEBUG << "Vehicle " << id << " is added: departed within region of interest" << endl;
            BasicNodeManager::addVehicle(id);
        } else {
            EV_DEBUG << "Vehicle " << id << " is not added: departed outside region of interest" << endl;
            m_outside.insert(id);
        }
    }
}

void RegionOfInterestNodeManager::removeVehicle(const std::string& id)
{
    auto found = m_outside.find(id);
    if (found == m_outside.end()) {
        BasicNodeManager::removeVehicle(id);
    } else {
        m_outside.erase(found);
    }
}

void RegionOfInterestNodeManager::updateVehicle(const std::string& id, VehicleSink* sink)
{
    if (m_regions.empty()) {
        BasicNodeManager::updateVehicle(id, sink);
    } else {
        /* check if vehicle is in Region of Interest */
        auto vehicle = getSubscriptions()->getVehicleCache(id);
        if (m_regions.cover(vehicle->get<libsumo::VAR_POSITION>())) {
            /* vehicle is known and in RoI */
            BasicNodeManager::updateVehicle(id, sink);
        } else {
            /* known vehicle left Region of Interest */
            EV_DEBUG << "Vehicle " << id << " was removed: left region of interest" << endl;
            BasicNodeManager::removeVehicle(id);
            m_outside.insert(id);
        }
    }
}

void RegionOfInterestNodeManager::checkRegionOfInterest()
{
    for (auto it = m_outside.begin(); it != m_outside.end();) {
        auto vehicle = getSubscriptions()->getVehicleCache(*it);
        if (m_regions.cover(vehicle->get<libsumo::VAR_POSITION>())) {
            EV_DEBUG << "Vehicle " << *it << " is added: entered region of interest" << endl;
            BasicNodeManager::addVehicle(*it);
            it = m_outside.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace traci
