/*
 * Artery V2X Simulation Framework
 * Copyright 2020 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "traci/InsertionDelayVehiclePolicy.h"
#include "traci/VehicleLifecycle.h"

using namespace omnetpp;

namespace traci
{

Define_Module(InsertionDelayVehiclePolicy);

InsertionDelayVehiclePolicy::InsertionDelayVehiclePolicy() :
    m_insert_event(new omnetpp::cMessage("TraCI vehicle insertion"))
{
}

InsertionDelayVehiclePolicy::~InsertionDelayVehiclePolicy()
{
    cancelAndDelete(m_insert_event);
}

void InsertionDelayVehiclePolicy::handleMessage(cMessage* msg)
{
    if (msg == m_insert_event && !m_insert_queue.empty()) {
        auto next_insertion = m_insert_queue.right.begin();
        const SimTime& when = next_insertion->first;
        const std::string& id = next_insertion->second;
        ASSERT(when == simTime());

        if (m_lifecycle) {
            m_lifecycle->addVehicle(id);
        } else {
            EV_FATAL << "no lifecycle interface registered\n";
        }

        m_insert_queue.right.erase(next_insertion);
        scheduleVehicleInsertion();
    }
}

void InsertionDelayVehiclePolicy::scheduleVehicleInsertion()
{
    cancelEvent(m_insert_event);
    if (!m_insert_queue.empty()) {
        auto next_insertion = m_insert_queue.right.begin();
        const SimTime& when = next_insertion->first;
        ASSERT(when >= simTime());
        scheduleAt(when, m_insert_event);
    }
}

void InsertionDelayVehiclePolicy::initialize(VehicleLifecycle* lifecycle)
{
    m_lifecycle = lifecycle;
}

VehiclePolicy::Decision InsertionDelayVehiclePolicy::addVehicle(const std::string& id)
{
    Enter_Method_Silent();
    const SimTime when = simTime() + par("insertionDelay");
    m_insert_queue.insert(InsertionQueue::relation(id, when));
    EV_DETAIL << "vehicle " << id << " will be inserted at time " << when << "\n";
    scheduleVehicleInsertion();
    return Decision::Discard;
}

VehiclePolicy::Decision InsertionDelayVehiclePolicy::updateVehicle(const std::string& id)
{
    Enter_Method_Silent();
    if (m_insert_queue.left.find(id) == m_insert_queue.left.end()) {
        return Decision::Continue;
    } else {
        return Decision::Discard;
    }
}

VehiclePolicy::Decision InsertionDelayVehiclePolicy::removeVehicle(const std::string& id)
{
    Enter_Method_Silent();
    const auto found = m_insert_queue.left.find(id);
    if (found == m_insert_queue.left.end()) {
        return Decision::Continue;
    } else {
        m_insert_queue.left.erase(found);
        scheduleVehicleInsertion();
        return Decision::Discard;
    }
}

} // namespace traci
