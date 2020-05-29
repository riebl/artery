#ifndef ARTERY_GTUMIDDLEWARE_H_RNDER8I0
#define ARTERY_GTUMIDDLEWARE_H_RNDER8I0

#include "artery/application/Middleware.h"
#include "artery/application/VehicleDataProvider.h"
#include "artery/application/VehicleKinematics.h"

namespace artery
{

class GtuInetMobility;

class GtuMiddleware : public Middleware
{
public:
    GtuMiddleware();
    void initialize(int stage) override;
    void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, omnetpp::cObject*, omnetpp::cObject*) override;

protected:
    void initializeStationType(const std::string&);
    VehicleKinematics getKinematics(const GtuInetMobility&) const;

private:
    VehicleDataProvider mVehicleDataProvider;
    GtuInetMobility* mMobility = nullptr;
};

} // namespace artery

#endif /* ARTERY_GTUMIDDLEWARE_H_RNDER8I0 */

