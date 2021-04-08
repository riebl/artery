/*
* Artery V2X Simulation Framework
* Copyright 2017 Thiago Vieira, Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/inet/gemv2/NLOSv.h"
#include "artery/inet/gemv2/Math.h"
#include "artery/inet/gemv2/VehicleIndex.h"
#include <inet/common/ModuleAccess.h>
#include <inet/common/Units.h>
#include <inet/physicallayer/contract/packetlevel/IRadioMedium.h>
#include <cmath>
#include <limits>
#include <vector>

namespace artery
{
namespace gemv2
{

Define_Module(NLOSv)

using namespace inet;
namespace phy = inet::physicallayer;

namespace {
auto compareDistance = [](const DiffractionObstacle& a, const DiffractionObstacle& b) { return a.d < b.d; };
auto compareHeight = [](const DiffractionObstacle& a, const DiffractionObstacle& b) { return a.h < b.h; };

using ObstacleIterator = std::list<DiffractionObstacle>::const_iterator;
ObstacleIterator findMainObstacle(ObstacleIterator, ObstacleIterator);
ObstacleIterator findSecondaryObstacle(ObstacleIterator, ObstacleIterator);
} // namespace

DiffractionObstacle::DiffractionObstacle(meter distTx, meter height) :
    d(distTx), h(height)
{
}

DiffractionPath::DiffractionPath() :
    attenuation(0.0), d(0.0)
{
}

NLOSv::NLOSv() :
    mVehicleIndex(nullptr)
{
}

void NLOSv::initialize()
{
    mVehicleIndex = inet::findModuleFromPar<VehicleIndex>(par("vehicleIndexModule"), this);
}

double NLOSv::computePathLoss(const phy::ITransmission* transmission, const phy::IArrival* arrival) const
{
    auto radioMedium = transmission->getTransmitter()->getMedium();
    auto narrowbandSignalAnalogModel = check_and_cast<const phy::INarrowbandSignal *>(transmission->getAnalogModel());
    const mps propagationSpeed = radioMedium->getPropagation()->getPropagationSpeed();
    const Hz carrierFrequency = Hz(narrowbandSignalAnalogModel->getCarrierFrequency());
    const m waveLength = propagationSpeed / carrierFrequency;
    const m distance { transmission->getStartPosition().distance(arrival->getStartPosition()) };

    double loss = 1.0;
    if (distance.get() != 0.0) {
        // free space loss
        loss = static_cast<inet::unit>(squared(waveLength) / (16.0 * squared(M_PI) * squared(distance))).get();
        // and additional attenuation by vehicles
        loss *= computeVehiclePathLoss(transmission->getStartPosition(), arrival->getStartPosition(), waveLength);
    }
    ASSERT(loss >= 0.0 && loss <= 1.0);
    return loss;
}

double NLOSv::computeVehiclePathLoss(const Coord& pos_tx, const Coord& pos_rx, m lambda) const
{
    const meter distTxRx { sqrt(squared(pos_rx.x - pos_tx.x) + squared(pos_rx.y - pos_tx.y)) }; /*< ground distance! */
    DiffractionObstacle Tx { meter(0.0), meter(pos_tx.z) };
    DiffractionObstacle Rx { distTxRx, meter(pos_rx.z) };

    auto vehicles = mVehicleIndex->getObstructingVehicles(Position { pos_tx.x, pos_tx.y }, Position { pos_rx.x, pos_rx.y });
    std::vector<DiffractionPath> paths;
    paths.reserve(3);

    auto obsTop = buildTopObstacles(vehicles, pos_tx, pos_rx);
    if (!obsTop.empty()) {
        obsTop.push_front(Tx);
        obsTop.push_back(Rx);
        paths.push_back(computeMultipleKnifeEdge(obsTop, lambda));
    }

    // original GEMV² code uses same Tx and Rx heights for all three paths: we assume zero "height" on side paths
    auto obsSides = buildSideObstacles(vehicles, pos_tx, pos_rx);
    Tx.h = meter(0.0);
    Rx.h = meter(0.0);
    if (!obsSides.left.empty()) {
        obsSides.left.push_front(Tx);
        obsSides.left.push_back(Rx);
        paths.push_back(computeMultipleKnifeEdge(obsSides.left, lambda));
    }
    if (!obsSides.right.empty()) {
        obsSides.right.push_front(Tx);
        obsSides.right.push_back(Rx);
        paths.push_back(computeMultipleKnifeEdge(obsSides.right, lambda));
    }

    return combineDiffractionLoss(paths, lambda);
}

double NLOSv::computePathLoss(mps propagationSpeed, Hz frequency, m distance) const
{
    return NaN;
}

m NLOSv::computeRange(mps propagationSpeed, Hz frequency, double loss) const
{
    return m(NaN);
}

double NLOSv::computeSimpleKnifeEdge(m heightTx, m heightRx, m heightObs, m distTxRx, m distTxObs, m lambda) const
{
    // following calculations are similar to equation 29 of ITU-R P.526-13:
    //  v = sqrt(2d/lambda * alpha1 * alpha2)
    //
    // with sinus approximation for small angles:
    //  alpha1 =~ sin alpha1 = h / d1
    //  alpha2 =~ sin alpha2 = h / d2
    //
    //  v = sqrt(2d / lambda * h / d1 * h / d2) = sqrt(2) * h / sqrt(lambda * d1 * d2 / d)
    //
    // with d1 = distTxObs, d2 = distRxObs, d = distTxRx,

    // distance between Rx and obstacle
    const m distRxObs = distTxRx - distTxObs;
    // signed height relative to the line connecting Tx and Rx at obstacle position
    const m obsHeightTxRxLine = (heightRx - heightTx) / distTxRx * distTxObs + heightTx;
    const m h = heightObs - obsHeightTxRxLine;
    // calculate the Fresnel ray
    const m r = sqrt(lambda * distTxObs * distRxObs / distTxRx);
    static const double root_two = sqrt(2.0);
    const double v = root_two * static_cast<inet::unit>(h / r).get();

    double loss = 0.0;
    if (v > -0.78) {
        // approximation of Fresnel-Kirchoff loss given by ITU-R P.526, equation 31 (result in dB):
        // J(v) = 6.9 + 20 log(sqrt((v - 01)^2 + 1) + v - 0.1)
        loss = 6.9 + 20.0 * log10(sqrt(squared(v - 0.1) + 1.0) + v - 0.1);
    }

    return loss;
}

DiffractionPath NLOSv::computeMultipleKnifeEdge(const std::list<DiffractionObstacle>& obs, m lambda) const
{
    ASSERT(obs.size() > 2);
    DiffractionPath path;

    // determine main and secondary obstacles
    std::vector<ObstacleIterator> mainObs;
    mainObs.push_back(obs.begin()); // Tx
    for (ObstacleIterator it = obs.begin(); it != obs.end();) {
        it = findMainObstacle(it, obs.end());
        if (it != obs.end()) {
            mainObs.push_back(it);
        }
    }
    // NOTE: Rx is added by loop as last main obstacle
    ASSERT(mainObs.size() <= obs.size());

    struct SecondaryObstacle {
        SecondaryObstacle(ObstacleIterator tx, ObstacleIterator obs, ObstacleIterator rx) :
            tx(tx), obstacle(obs), rx(rx) {}
        ObstacleIterator tx;
        ObstacleIterator obstacle;
        ObstacleIterator rx;
    };

    std::vector<SecondaryObstacle> secObs;
    std::vector<meter> mainObsDistances;
    for (std::size_t i = 0, j = 1; j < mainObs.size(); ++i, ++j) {
        const meter d = mainObs[j]->d - mainObs[i]->d;
        path.d += sqrt(squared(d) + squared(mainObs[j]->h - mainObs[i]->h));
        mainObsDistances.push_back(d);

        const auto delta = std::distance(mainObs[i], mainObs[j]);
        if (delta == 2) {
            // single other obstacle between two main obstacles
            secObs.emplace_back(mainObs[i], std::next(mainObs[i]), mainObs[j]);
        } else if (delta > 2) {
            secObs.emplace_back(mainObs[i], findSecondaryObstacle(mainObs[i], mainObs[j]), mainObs[j]);
        }
    }

    // attenuation due to main obstacles
    double attMainObs = 0.0;
    for (std::size_t i = 0; i < mainObs.size() - 2; ++i) {
        const meter distTxObs = mainObsDistances[i];
        const meter distTxRx = distTxObs + mainObsDistances[i+1];
        attMainObs += computeSimpleKnifeEdge(mainObs[i]->h, mainObs[i+2]->h, mainObs[i+1]->h, distTxRx, distTxObs, lambda);
    }

    // attenuation due to secondary obstacles
    double attSecObs = 0.0;
    for (const SecondaryObstacle& sec : secObs) {
        const meter distTxRx = sec.rx->d - sec.tx->d;
        const meter distTxObs = sec.obstacle->d - sec.tx->d;
        attSecObs += computeSimpleKnifeEdge(sec.tx->h, sec.rx->h, sec.obstacle->h, distTxRx, distTxObs, lambda);
    }

    // correction factor C (see eq. 46 in ITU-R P.526-13)
    double C = (mainObs.back()->d - mainObs.front()->d).get(); // distance between Tx and Rx
    for (meter d : mainObsDistances) {
        C *= d.get();
    }
    double pairwiseDistProduct = 1.0;
    for (std::size_t i = 1; i < mainObsDistances.size(); ++i) {
        pairwiseDistProduct *= (mainObsDistances[i-1] + mainObsDistances[i]).get();
    }
    C /= mainObsDistances.front().get() * mainObsDistances.back().get() * pairwiseDistProduct;

    path.attenuation = inet::math::dB2fraction(attMainObs + attSecObs) / C;
    ASSERT(path.attenuation >= 1.0);
    return path;
}

std::list<DiffractionObstacle> NLOSv::buildTopObstacles(const VehicleList& vehicles, const Coord& pos_tx, const Coord& pos_rx) const
{
    std::list<DiffractionObstacle> diffTop;
    const double vx = pos_rx.x - pos_tx.x;
    const double vy = pos_rx.y - pos_tx.y;

    for (const VehicleIndex::Vehicle* vehicle : vehicles) {
        const Position& midpoint = vehicle->getMidpoint();
        const double k = (midpoint.x.value() * vx - vy * pos_tx.y + vy * midpoint.y.value() - pos_tx.x * vx) / (squared(vx) + squared(vy));
        if (k < 0.0 || k > 1.0) continue; /*< skip points beyond the ends of TxRx line segment */
        const double d = k * sqrt(squared(vx) + squared(vy));
        diffTop.emplace_back(meter(d), meter(vehicle->getHeight()));
    }

    diffTop.sort(compareDistance);
    return diffTop;
}

NLOSv::SideObstacles NLOSv::buildSideObstacles(const VehicleList& vehicles, const Coord& pos_tx, const Coord& pos_rx) const
{
    std::list<DiffractionObstacle> diffOnRightSide;
    std::list<DiffractionObstacle> diffOnLeftSide;

    //constexpr double NaN = std::numeric_limits<double>::quiet_NaN();
    constexpr double Inf = std::numeric_limits<double>::infinity();

    // x and y components of vector v pointing from transmitter to receiver position
    const double vx = pos_rx.x - pos_tx.x;
    const double vy = pos_rx.y - pos_tx.y;
    const double vl = sqrt(squared(vx) + squared(vy));

    struct Intersection {
        double k;
        double l;
    };

    for (const VehicleIndex::Vehicle* vehicle : vehicles) {
        const std::vector<Position>& outline = vehicle->getOutline();
        Intersection leftmost { NaN, -Inf };
        Intersection rightmost { NaN, Inf };

        using OutlineIterator = std::vector<Position>::const_iterator;
        for (OutlineIterator it = outline.begin(); it != outline.end(); ++it) {
            const Coord p { it->x.value(), it->y.value() };
            Intersection current;
            current.k = (p.x * vx - vy * pos_tx.y + vy * p.y - pos_tx.x * vx) / (squared(vx) + squared(vy));
            if (vx != 0.0) {
                current.l = (pos_tx.y + current.k * vy - p.y) / vx;
            } else if (vy != 0.0) {
                current.l = (pos_tx.x + current.k * vx - p.x) / -vy;
            } else {
                throw cRuntimeError("vx and vy are zero");
            }

            if (current.k < 0.0 || current.k > 1.0) {
               // skip this point because it is beyond the ends of the TxRx line segment
               continue;
            }

            if (current.l > 0.0) {
                if (leftmost.l < current.l) {
                    leftmost = current;
                }
            } else if (current.l < 0.0) {
                if (rightmost.l > current.l) {
                    rightmost = current;
                }
            } else {
                // exactly on TxRx line -> possibly left or right maximum (even both)
                if (leftmost.l < current.l) {
                    leftmost = current;
                }
                if (rightmost.l > current.l) {
                    rightmost = current;
                }
            }
        }

        if (leftmost.l > -Inf) {
            diffOnLeftSide.emplace_back(meter(leftmost.k * vl), meter(leftmost.l * vl));
        }
        if (rightmost.l < Inf) {
            diffOnRightSide.emplace_back(meter(rightmost.k * vl), meter(-rightmost.l * vl));
        }
    }

    diffOnLeftSide.sort(compareDistance);
    diffOnRightSide.sort(compareDistance);
    return SideObstacles { std::move(diffOnLeftSide), std::move(diffOnRightSide) };
}

double NLOSv::combineDiffractionLoss(const std::vector<DiffractionPath>& paths, m /* lambda */) const
{
    ASSERT(!paths.empty());

    // use only smallest diffraction attenuation (maximum E-field)
    double min_attenuation = std::numeric_limits<double>::infinity();
    for (const DiffractionPath& path : paths) {
        if (path.attenuation < min_attenuation) {
            min_attenuation = path.attenuation;
        }
    }

    return 1.0 / min_attenuation;
}

namespace {

ObstacleIterator findMainObstacle(ObstacleIterator begin, ObstacleIterator end)
{
    ObstacleIterator mainIterator = end;
    double mainAngle = -std::numeric_limits<double>::infinity();

    if (begin != end) {
        for (ObstacleIterator it = std::next(begin); it != end; ++it) {
            double angle = static_cast<inet::unit>((it->h - begin->h) / (it->d - begin->d)).get();
            if (angle > mainAngle) {
                mainIterator = it;
                mainAngle = angle;
            }
        }
    }

    return mainIterator;
}

ObstacleIterator findSecondaryObstacle(ObstacleIterator first, ObstacleIterator last)
{
    ASSERT(std::distance(first, last) > 2);
    ObstacleIterator secIterator = last;
    inet::m secHeightGap { std::numeric_limits<double>::infinity() };

    const inet::m distFirstLast = last->d - first->d;
    const inet::m heightFirstLast = last->h - first->h;
    const auto offset = first->h * last->d - first->d * last->h;
    for (ObstacleIterator it = std::next(first); it != last; ++it) {
        const inet::m heightGap = ((it->d * heightFirstLast + offset) / distFirstLast) - it->h;
        if (heightGap < secHeightGap) {
            secIterator = it;
            secHeightGap = heightGap;
        }
    }

    return secIterator;
}

} // namespace

} // namespace gemv2
} // namespace artery
