#ifndef ARTERY_GEMV2_NLOSV_H_NV3WEACB
#define ARTERY_GEMV2_NLOSV_H_NV3WEACB

#include "artery/inet/gemv2/VehicleIndex.h"
#include <inet/common/Units.h>
#include <inet/physicallayer/contract/packetlevel/IPathLoss.h>
#include <omnetpp/csimplemodule.h>
#include <list>

namespace artery
{
namespace gemv2
{

using meter = inet::units::values::m;

struct DiffractionObstacle
{
    DiffractionObstacle(meter distTx, meter height);

    meter d; // distance to transmitter
    meter h; // height of obstacle
};

struct DiffractionPath
{
    DiffractionPath();

    double attenuation;
    meter d;
};

class NLOSv : public omnetpp::cSimpleModule, public inet::physicallayer::IPathLoss
{
public:
    NLOSv();
    void initialize() override;
    double computePathLoss(const inet::physicallayer::ITransmission*, const inet::physicallayer::IArrival*) const override;
    double computePathLoss(inet::mps propagation, inet::Hz frequency, inet::m distance) const override;
    inet::m computeRange(inet::mps propagation, inet::Hz frequency, double loss) const override;

protected:
    using VehicleList = std::vector<const VehicleIndex::Vehicle*>;


    struct SideObstacles {
        std::list<DiffractionObstacle> left;
        std::list<DiffractionObstacle> right;
    };

    virtual double computeVehiclePathLoss(const inet::Coord&, const inet::Coord&, inet::m lambda) const;
    virtual DiffractionPath computeMultipleKnifeEdge(const std::list<DiffractionObstacle>&, inet::m lambda) const;
    virtual double computeSimpleKnifeEdge(inet::m heightTx, inet::m heightRx, inet::m heightObs, inet::m distTxRx, inet::m distTxObs, inet::m lambda) const;
    virtual std::list<DiffractionObstacle> buildTopObstacles(const VehicleList&, const inet::Coord& tx, const inet::Coord& rx) const;
    virtual SideObstacles buildSideObstacles(const VehicleList&, const inet::Coord& tx, const inet::Coord& rx) const;
    virtual double combineDiffractionLoss(const std::vector<DiffractionPath>&, inet::m lambda) const;

    const VehicleIndex* mVehicleIndex;
};

} // namespace gemv2
} // namespace artery

#endif /* ARTERY_GEMV2_NLOSV_H_NV3WEACB */
