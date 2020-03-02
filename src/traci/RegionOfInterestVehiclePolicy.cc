#include "traci/RegionOfInterestVehiclePolicy.h"
#include "traci/BasicNodeManager.h"
#include "traci/LiteAPI.h"
#include "traci/VariableCache.h"
#include "traci/VehicleLifecycle.h"
#include <omnetpp/cxmlelement.h>
#include <cassert>

using namespace omnetpp;

namespace traci
{

Define_Module(RegionOfInterestVehiclePolicy)

void RegionOfInterestVehiclePolicy::initialize(VehicleLifecycle* lifecycle)
{
    BasicNodeManager* manager = dynamic_cast<BasicNodeManager*>(getParentModule());
    if (!manager) {
        throw cRuntimeError("Missing traci::BasicNodeManager as parent module");
    }

    /* validate regions */
    cXMLElement* regions = par("regionsOfInterest").xmlValue();
    if (regions) {
        Boundary boundary { manager->getLiteAPI()->simulation().getNetBoundary() };
        m_regions.initialize(*regions, boundary);
        EV_INFO << "Added " << m_regions.size() << " Regions of Interest to simulation" << endl;
    }

    m_lifecycle = lifecycle;
    m_subscriptions = manager->getSubscriptions();
    manager->subscribe(BasicNodeManager::updateNodeSignal, this);
}

void RegionOfInterestVehiclePolicy::receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t signal, unsigned long n, omnetpp::cObject*)
{
    if (signal == BasicNodeManager::updateNodeSignal) {
        checkRegionOfInterest();
    }
}

VehiclePolicy::Decision RegionOfInterestVehiclePolicy::addVehicle(const std::string& id)
{
    assert(m_subscriptions);

    if (m_regions.empty()) {
        return Decision::Continue;
    } else {
        /* check if vehicle is in Region of Interest */
        auto vehicle = m_subscriptions->getVehicleCache(id);
        if (m_regions.cover(vehicle->get<libsumo::VAR_POSITION>())) {
            /* vehicle was in region and NOT in vehicle list */
            EV_DEBUG << "Vehicle " << id << " is added: departed within region of interest" << endl;
            return Decision::Continue;
        } else {
            EV_DEBUG << "Vehicle " << id << " is not added: departed outside region of interest" << endl;
            m_outside.insert(id);
            return Decision::Discard;
        }
    }
}

VehiclePolicy::Decision RegionOfInterestVehiclePolicy::updateVehicle(const std::string& id)
{
    assert(m_subscriptions);
    assert(m_lifecycle);

    if (m_regions.empty()) {
        return Decision::Continue;
    } else {
        /* check if vehicle is in Region of Interest */
        auto vehicle = m_subscriptions->getVehicleCache(id);
        if (m_regions.cover(vehicle->get<libsumo::VAR_POSITION>())) {
            /* vehicle is known and in RoI */
            return Decision::Continue;
        } else {
            /* known vehicle left Region of Interest */
            EV_DEBUG << "Vehicle " << id << " was removed: left region of interest" << endl;
            m_lifecycle->removeVehicle(id);
            m_outside.insert(id);
            return Decision::Discard;
        }
    }
}

VehiclePolicy::Decision RegionOfInterestVehiclePolicy::removeVehicle(const std::string& id)
{
    auto found = m_outside.find(id);
    if (found == m_outside.end()) {
        return Decision::Continue;
    } else {
        m_outside.erase(found);
        return Decision::Discard;
    }
}

void RegionOfInterestVehiclePolicy::checkRegionOfInterest()
{
    assert(m_subscriptions);
    assert(m_lifecycle);

    for (auto it = m_outside.begin(); it != m_outside.end();) {
        auto vehicle = m_subscriptions->getVehicleCache(*it);
        if (m_regions.cover(vehicle->get<libsumo::VAR_POSITION>())) {
            EV_DEBUG << "Vehicle " << *it << " is added: entered region of interest" << endl;
            m_lifecycle->addVehicle(*it);
            it = m_outside.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace traci
