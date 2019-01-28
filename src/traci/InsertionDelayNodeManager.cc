/*
 * Artery V2X Simulation Framework
 * Copyright 2018 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "traci/InsertionDelayNodeManager.h"

using namespace omnetpp;

namespace traci
{

Define_Module(InsertionDelayNodeManager)

InsertionDelayNodeManager::InsertionDelayNodeManager() :
    m_insert_event(new omnetpp::cMessage("TraCI vehicle insertion"))
{
}

InsertionDelayNodeManager::~InsertionDelayNodeManager()
{
    cancelAndDelete(m_insert_event);
}

void InsertionDelayNodeManager::handleMessage(cMessage* msg)
{
    if (msg == m_insert_event && !m_insert_queue.empty()) {
        auto next_insertion = m_insert_queue.right.begin();
        const SimTime& when = next_insertion->first;
        const std::string& id = next_insertion->second;
        ASSERT(when == simTime());

        BasicNodeManager::addVehicle(id);
        BasicNodeManager::updateVehicle(id, getVehicleSink(id));
        m_insert_queue.right.erase(next_insertion);
        scheduleVehicleInsertion();
    } else {
        BasicNodeManager::handleMessage(msg);
    }
}

void InsertionDelayNodeManager::scheduleVehicleInsertion()
{
    cancelEvent(m_insert_event);
    if (!m_insert_queue.empty()) {
        auto next_insertion = m_insert_queue.right.begin();
        const SimTime& when = next_insertion->first;
        ASSERT(when >= simTime());
        scheduleAt(when, m_insert_event);
    }
}

void InsertionDelayNodeManager::addVehicle(const std::string& id)
{
    Enter_Method_Silent();
    const SimTime when = simTime() + par("insertionDelay");
    m_insert_queue.insert(InsertionQueue::relation(id, when));
    EV_DETAIL << "vehicle " << id << " will be inserted at time " << when << "\n";
    scheduleVehicleInsertion();
}

void InsertionDelayNodeManager::updateVehicle(const std::string& id, VehicleSink* sink)
{
    Enter_Method_Silent();
    if (m_insert_queue.left.find(id) == m_insert_queue.left.end()) {
        BasicNodeManager::updateVehicle(id, sink);
    }
}

void InsertionDelayNodeManager::removeVehicle(const std::string& id)
{
    Enter_Method_Silent();
    const auto found = m_insert_queue.left.find(id);
    if (found == m_insert_queue.left.end()) {
        BasicNodeManager::removeVehicle(id);
    } else {
        m_insert_queue.left.erase(found);
        scheduleVehicleInsertion();
    }
}

} // namespace traci
