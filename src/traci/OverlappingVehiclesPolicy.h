#ifndef OVERLAPPINGVEHICLESPOLICY_2323
#define OVERLAPPINGVEHICLESPOLICY_2323

#include "traci/BasicNodeManager.h"
#include "traci/VehiclePolicy.h"
#include "traci/VehicleLifecycle.h"
#include "traci/Position.h"
#include <unordered_map>
#include <unordered_set>
#include <omnetpp/clistener.h>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>

namespace traci
{

class SubscriptionManager;

class OverlappingVehiclesPolicy : public VehiclePolicy, public omnetpp::cListener
{
public:
    void initialize(VehicleLifecycle*) override;
    Decision addVehicle(const std::string& id) override;
    Decision updateVehicle(const std::string& id) override;
    Decision removeVehicle(const std::string& id) override;
    Decision startVehicleParking(const std::string& id) override;
    Decision endVehicleParking(const std::string& id) override;

protected:
    void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, unsigned long n, omnetpp::cObject*) override;

private:
    using Point =  boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>;
    using Box = boost::geometry::model::box<Point>;
    typedef std::pair<Point, std::string> value;

    using RTree = boost::geometry::index::rtree<value, boost::geometry::index::quadratic<16>>;

    bool checkIfOverlapping(const std::string id);

    const TraCIPosition& getPosition(const std::string& id);
    void addToPositionTree(const std::string& id);
    void removeFromPositionTree(const std::string& id);
    void bulkConstructRtree();

    RTree m_rtree;

    std::unordered_set<std::string> m_removed_vehicles;

    std::unordered_set<std::string> m_check_ids;

    bool m_initial_run_is_done = false;

    BasicNodeManager* m_manager;
    SubscriptionManager* m_subscription_manager;
    VehicleLifecycle* m_vehicle_lifecycle;
};

} // namespace traci

#endif /* OVERLAPPINGVEHICLESPOLICY_2323 */

