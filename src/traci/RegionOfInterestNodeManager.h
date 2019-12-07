/*
 * Artery V2X Simulation Framework
 * Copyright 2019 Alexander Willecke, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef REGIONOFINTERESTNODEMANAGER_H_AGMBROIE
#define REGIONOFINTERESTNODEMANAGER_H_AGMBROIE

#include "traci/BasicNodeManager.h"
#include "traci/RegionsOfInterest.h"
#include <unordered_set>

namespace traci
{

class RegionOfInterestNodeManager : public BasicNodeManager
{
protected:
    void traciInit() override;

    void addVehicle(const std::string& id) override;
    void removeVehicle(const std::string& id) override;
    void updateVehicle(const std::string& id, VehicleSink* sink) override;
    void processVehicles() override;

private:
    void checkRegionOfInterest();

    RegionsOfInterest m_regions;
    std::unordered_set<std::string> m_outside;
};

} // namespace traci

#endif /* REGIONOFINTERESTNODEMANAGER_H_AGMBROIE */

