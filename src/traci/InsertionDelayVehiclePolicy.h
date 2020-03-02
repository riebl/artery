/*
 * Artery V2X Simulation Framework
 * Copyright 2020 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef TRACI_INSERTIONDELAYVEHICLEPOLICY_H_CULRKQOD
#define TRACI_INSERTIONDELAYVEHICLEPOLICY_H_CULRKQOD

#include "traci/VehiclePolicy.h"
#include <boost/bimap/bimap.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <omnetpp/cmessage.h>
#include <omnetpp/simtime.h>

namespace traci
{

/**
 * This policy inserts vehicles only after some delay.
 */
class InsertionDelayVehiclePolicy : public VehiclePolicy
{
public:
    InsertionDelayVehiclePolicy();
    ~InsertionDelayVehiclePolicy();

    void initialize(VehicleLifecycle*) override;
    Decision addVehicle(const std::string& id) override;
    Decision updateVehicle(const std::string& id) override;
    Decision removeVehicle(const std::string& id) override;

protected:
    void handleMessage(omnetpp::cMessage* msg) override;

private:
    using InsertionQueue = boost::bimaps::bimap<std::string, boost::bimaps::multiset_of<omnetpp::SimTime>>;

    void scheduleVehicleInsertion();

    VehicleLifecycle* m_lifecycle = nullptr;
    omnetpp::cMessage* m_insert_event = nullptr;
    InsertionQueue m_insert_queue;
};

} // namespace traci

#endif /* TRACI_INSERTIONDELAYVEHICLEPOLICY_H_CULRKQOD */

