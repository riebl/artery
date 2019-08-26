/*
 * Artery V2X Simulation Framework
 * Copyright 2018 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef INSERTIONDELAYNODEMANAGER_H_TJPHDLYB
#define INSERTIONDELAYNODEMANAGER_H_TJPHDLYB

#include "traci/BasicNodeManager.h"
#include <boost/bimap/bimap.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <omnetpp/cmessage.h>
#include <omnetpp/simtime.h>

namespace traci
{

class InsertionDelayNodeManager : public BasicNodeManager
{
public:
    InsertionDelayNodeManager();
    ~InsertionDelayNodeManager();

protected:
    void handleMessage(omnetpp::cMessage*) override;

    void addVehicle(const std::string& id) override;
    void updateVehicle(const std::string& id, VehicleSink*) override;
    void removeVehicle(const std::string& id) override;

private:
    using InsertionQueue = boost::bimaps::bimap<std::string, boost::bimaps::multiset_of<omnetpp::SimTime>>;

    void scheduleVehicleInsertion();

    omnetpp::cMessage* m_insert_event;
    InsertionQueue m_insert_queue;
};

} // namespace traci

#endif /* INSERTIONDELAYNODEMANAGER_H_TJPHDLYB */

