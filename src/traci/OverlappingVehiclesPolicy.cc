#include "OverlappingVehiclesPolicy.h"
#include "BasicNodeManager.h"
#include "traci/VariableCache.h"
#include "inet/common/ModuleAccess.h"
#include <cmath>

using namespace omnetpp;

namespace traci
{

Define_Module(OverlappingVehiclesPolicy)


void OverlappingVehiclesPolicy::initialize(VehicleLifecycle* vehicleLifecycle) {
    m_vehicle_lifecycle = vehicleLifecycle;

    m_manager = inet::getModuleFromPar<BasicNodeManager>(par("basicNodeManager"), this);
    m_subscription_manager = m_manager->getSubscriptions();

    m_manager->subscribe(BasicNodeManager::updateNodeSignal, this);
}

const TraCIPosition& OverlappingVehiclesPolicy::getPosition(const std::string& id) {
    // get the postion of the vehicle with the given id
    auto& vehicle = *m_subscription_manager->getVehicleCache(id);
    return vehicle.get<libsumo::VAR_POSITION>();
}

void OverlappingVehiclesPolicy::addToPositionTree(const std::string& id) {
    // add vehicle with the given id to the position R-tree
    auto& position = getPosition(id);
    m_rtree.insert(std::make_pair(Point(position.x, position.y), id));
}

void OverlappingVehiclesPolicy::removeFromPositionTree(const std::string& id) {
    // remove vehicle with the given id from the position R-tree
    auto& position = getPosition(id);
    m_rtree.remove(std::make_pair(Point(position.x, position.y), id));
}

void OverlappingVehiclesPolicy::receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t signal, unsigned long n, omnetpp::cObject* obj)
{
    if (signal == BasicNodeManager::updateNodeSignal) {
        if (!m_initial_run_is_done) {
            // clear position tracking r-tree
            m_rtree.clear();
            // after the initial removal step, only upon a vehicle starting to
            // park the r-tree is fully rebuild, otherwise it is not used
            m_initial_run_is_done = true;
        }
    }
}

bool OverlappingVehiclesPolicy::checkIfOverlapping(const std::string id) {
    // get the position of the vehicle
    auto& position = getPosition(id);

    // using 2D points only for now
    if (std::fpclassify(position.z) != FP_ZERO) {
        throw cRuntimeError("z position is not 0");
    }

    Box dedup_box(Point(position.x - 1.0, position.y - 1.0), Point(position.x + 1.0, position.y + 1.0));

    std::vector<value> result;
    // query the r-tree for other vehicles within the bounding box
    m_rtree.query(boost::geometry::index::within(dedup_box), std::back_inserter(result));

    if (result.size() != 0) {
        // there are already vehicles at this position
        for (auto r : result) {
            if (r.second.compare(id) == 0) {
                // same vehicle
                continue;
            } else {
                return true;
            }

        }
        // there is no overlap if only the same vehicle is in this position
        return false;
    } else {
        return false;
    }
}

traci::VehiclePolicy::Decision OverlappingVehiclesPolicy::addVehicle(const std::string& id)
{
    bool is_overlapping = checkIfOverlapping(id);
    if (is_overlapping) {
        auto position = getPosition(id);
        m_removed_vehicles.erase(id);
        return traci::VehiclePolicy::Decision::Discard;
    } else {
        if (!m_initial_run_is_done) {
            // add vehicle to position tree
            addToPositionTree(id);
        }

        // remove from set of removed vehicles
        m_removed_vehicles.erase(id);
        // add to set of vehicles to check overlap with
        m_check_ids.insert(id);

        return traci::VehiclePolicy::Decision::Continue;
    }
}


OverlappingVehiclesPolicy::Decision OverlappingVehiclesPolicy::removeVehicle(const std::string& id)
{
    // don't check for overlap with this vehicle anymore
    m_check_ids.erase(id);

    return traci::VehiclePolicy::Decision::Continue;
}

traci::VehiclePolicy::Decision OverlappingVehiclesPolicy::updateVehicle(const std::string& id)
{
    return traci::VehiclePolicy::Decision::Continue;
}

void OverlappingVehiclesPolicy::bulkConstructRtree() {
    std::vector<value> data;
    // prepare position data for bulk insertion into r-tree
    for (auto id : m_check_ids) {
        if (m_removed_vehicles.find(id) == m_removed_vehicles.end()) {
            auto position = getPosition(id);
            auto value = std::make_pair(Point(position.x, position.y), id);
            data.push_back(value);
        }
    }

    // construct a new r-tree and bulk insert the positions of the desired vehicles
    m_rtree = RTree(data);
}

OverlappingVehiclesPolicy::Decision OverlappingVehiclesPolicy::startVehicleParking(const std::string& id)
{
    // construct r-tree with fresh position data
    bulkConstructRtree();

    if (checkIfOverlapping(id)) {
        // there is already a vehicle parked at this location, so remove this
        // vehicle and remember its id
        m_vehicle_lifecycle->removeVehicle(id);
        m_removed_vehicles.insert(id);
    }

    // clear out soon to be stale position data
    m_rtree.clear();

    auto position = getPosition(id);

    return traci::VehiclePolicy::Decision::Continue;
}

OverlappingVehiclesPolicy::Decision OverlappingVehiclesPolicy::endVehicleParking(const std::string& id)
{
    // don't check newly parking vehicles for overlap with this vehicle
    m_check_ids.erase(id);

    // check whether this vehicle had been removed before to prevent adding a vehicle twice
    auto removal_match = m_removed_vehicles.find(id);
    if (removal_match != m_removed_vehicles.end()) {
        // vehicle has been removed before and can be readded
        m_vehicle_lifecycle->addVehicle(id);
        m_removed_vehicles.erase(id);

        auto position = getPosition(id);
    }

    return traci::VehiclePolicy::Decision::Continue;
}

}
