#ifndef REGIONOFINTERESTVEHICLEPOLICY_H_TNK4CWW6
#define REGIONOFINTERESTVEHICLEPOLICY_H_TNK4CWW6

#include "traci/RegionsOfInterest.h"
#include "traci/VehiclePolicy.h"
#include <unordered_set>
#include <omnetpp/clistener.h>

namespace traci
{

class SubscriptionManager;

class RegionOfInterestVehiclePolicy : public VehiclePolicy, public omnetpp::cListener
{
public:
    void initialize(VehicleLifecycle*) override;
    Decision addVehicle(const std::string& id) override;
    Decision updateVehicle(const std::string& id) override;
    Decision removeVehicle(const std::string& id) override;

protected:
    void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, unsigned long n, omnetpp::cObject*) override;

private:
    void checkRegionOfInterest();

    SubscriptionManager* m_subscriptions;
    VehicleLifecycle* m_lifecycle;
    RegionsOfInterest m_regions;
    std::unordered_set<std::string> m_outside;
};

} // namespace traci

#endif /* REGIONOFINTERESTVEHICLEPOLICY_H_TNK4CWW6 */

