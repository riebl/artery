/*
* Artery V2X Simulation Framework
* Copyright 2017 Thiago Vieira, Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/inet/gemv2/NLOSb.h"
#include "artery/inet/gemv2/Math.h"
#include "artery/inet/gemv2/ObstacleIndex.h"
#include "artery/inet/gemv2/VehicleIndex.h"
#include "artery/inet/gemv2/Visualizer.h"
#include <inet/common/ModuleAccess.h>
#include <inet/common/Units.h>
#include <inet/physicallayer/contract/packetlevel/IPathLoss.h>
#include <inet/physicallayer/contract/packetlevel/IRadioMedium.h>
#include <boost/geometry.hpp>
#include <boost/geometry/views/closeable_view.hpp>
#include <boost/units/cmath.hpp>
#include <algorithm>
#include <cmath>
#include <vector>

using namespace inet;
namespace phy = inet::physicallayer;
namespace bg = boost::geometry;

namespace artery
{
namespace gemv2
{

using PositionSegment = bg::model::segment<Position>;
using PositionRange = std::vector<Position>;
using PositionView = bg::closeable_view<const PositionRange, bg::closure<PositionRange>::value>::type;

namespace
{

double squaredLength(const Position& a, const Position& b)
{
    return squared(a.x.value() - b.x.value()) + squared(a.y.value() - b.y.value());
}

Position reflectPoint(const Position& p, const PositionSegment& line)
{
    double dx = bg::get<1, 0>(line) - bg::get<0, 0>(line);
    double dy = bg::get<1, 1>(line) - bg::get<0, 1>(line);
    double a = (dx * dx - dy * dy) / (dx * dx + dy * dy);
    double b = 2.0 * dx * dy / (dx * dx + dy * dy);
    Position q;
    q.x = a * (p.x - line.first.x) + b * (p.y - line.first.y) + line.first.x;
    q.y = b * (p.x - line.first.x) - a * (p.y - line.first.y) + line.first.y;
    return q;
}

inet::m getWaveLength(const inet::physicallayer::ITransmission* transmission)
{
    auto radioMedium = transmission->getTransmitter()->getMedium();
    auto narrowbandSignalAnalogModel = check_and_cast<const phy::INarrowbandSignal *>(transmission->getAnalogModel());
    const mps propagationSpeed = radioMedium->getPropagation()->getPropagationSpeed();
    const Hz carrierFrequency = Hz(narrowbandSignalAnalogModel->getCarrierFrequency());
    const m waveLength = propagationSpeed / carrierFrequency;
    return waveLength;
}

} // namespace


Define_Module(NLOSb)

void NLOSb::initialize()
{
    mObstacleIndex = inet::findModuleFromPar<ObstacleIndex>(par("obstacleIndexModule"), this);
    mVehicleIndex = inet::findModuleFromPar<VehicleIndex>(par("vehicleIndexModule"), this);

    maxRange = par("maxRange");
    pathLossExponent = par("pathLossExponent");
    vehReflRelPerm = par("vehicleRelativePermittivity");
    obsReflRelPerm = par("buildingRelativePermittivity");

    const std::string polarization_str = par("polarization");
    if (polarization_str == "horizontal") {
        polarization = 'h';
    } else if (polarization_str == "vertical") {
        polarization = 'v';
    } else {
        error("Invalid antenna polarization %s", polarization_str.c_str());
    }

    mVisualizer = inet::findModuleFromPar<Visualizer>(par("visualizerModule"), this, false);
}

NLOSb::NLOSb() :
    mVehicleIndex(nullptr), mObstacleIndex(nullptr),
    maxRange(0.0), pathLossExponent(2.0), vehReflRelPerm(1.0), obsReflRelPerm(1.0)
{
}

double NLOSb::computePathLoss(mps propagationSpeed, Hz frequency, m distance) const
{
    return NaN;
}

m NLOSb::computeRange(mps propagationSpeed, Hz frequency, double loss) const
{
    return m(NaN);
}

double NLOSb::computePathLoss(const phy::ITransmission* transmission, const phy::IArrival* arrival) const
{
    const Environment env(this, transmission->getStartPosition(), arrival->getStartPosition(), getWaveLength(transmission));
    const inet::m distRxTx { transmission->getStartPosition().distance(arrival->getStartPosition()) };

    std::vector<Position> reflBuildings = computeReflectionRaysFromBuildings(env);
    std::vector<Attenuation> attReflBuildings = computeReflectionAttenuation(reflBuildings, obsReflRelPerm, env);

    std::vector<Position> reflVehicles = computeReflectionRaysFromVehicles(env);
    std::vector<Attenuation> attReflVehicles = computeReflectionAttenuation(reflVehicles, vehReflRelPerm, env);

    if (mVisualizer) {
        mVisualizer->drawReflectionRays(env.tx, env.rx, reflBuildings, reflVehicles);
    }

    // shortest reflection ray is upper bound for feasible diffraction rays
    Length limit = maxRange * boost::units::si::meter;
    for (const Attenuation& att : attReflBuildings)
    {
        if (att.length < limit) {
            limit = att.length;
        }
    }
    for (const Attenuation& att : attReflVehicles)
    {
        if (att.length < limit) {
            limit = att.length;
        }
    }

    auto diffractions = computeDiffractionRays(env);
    auto attenuations = computeDiffractionAttenuation(diffractions, limit, env);
    attenuations.reserve(attenuations.size() + attReflBuildings.size() + attReflVehicles.size());
    attenuations.insert(attenuations.end(), attReflBuildings.begin(), attReflBuildings.end());
    attenuations.insert(attenuations.end(), attReflVehicles.begin(), attReflVehicles.end());

    auto refldif = combineAttenuations(attenuations, env.lambda);
    auto logdist = computeLogDistanceLoss(distRxTx, env.lambda);
    return std::max(refldif, logdist);
}

std::vector<Position> NLOSb::computeReflectionRaysFromBuildings(const Environment& env) const
{
    std::vector<Position> rays;
    std::vector<Position> intersections;
    const double squaredLengthTxRx = squaredLength(env.tx, env.rx);

    for (const ObstacleIndex::Obstacle* obstacle : env.obstacles)
    {
        const PositionRange& outline = obstacle->getOutline();
        if (outline.size() < 2) continue;

        PositionView view(outline);
        for (auto a = view.begin(), b = std::next(a); b != view.end(); ++a, ++b)
        {
            PositionSegment segment(*a, *b);

            // calculate mirror point Rx' of Rx w.r.t. segment
            Position rx_m = reflectPoint(env.rx, segment);

            // no valid reflection possible if d(Tx, Rx) > d(Tx, Rx')
            if (squaredLengthTxRx > squaredLength(env.tx, rx_m)) {
                // skip segment because Tx and Rx are on opposite sides of segment
                continue;
            }

            // intersection between ray TxRx' and segment is reflection point
            PositionSegment ray(env.tx, rx_m);
            bg::intersection(segment, ray, intersections);
            if (intersections.size() == 1 && !isRayObstructed(intersections[0], env)) {
                rays.emplace_back(intersections[0]);
            }
            intersections.clear();
        }
    }

    return rays;
}

std::vector<Position> NLOSb::computeReflectionRaysFromVehicles(const Environment& env) const
{
    std::vector<Position> rays;
    std::vector<Position> intersections;
    const double squaredLengthTxRx = squaredLength(env.tx, env.rx);

    for (const VehicleIndex::Vehicle* vehicle : env.vehicles)
    {
        const PositionRange& outline = vehicle->getOutline();
        if (outline.size() < 2) continue;

        PositionView view(outline);
        for (auto a = view.begin(), b = std::next(a); b != view.end(); ++a, ++b)
        {
            PositionSegment segment(*a, *b);

            // calculate mirror point Rx' of Rx w.r.t. segment
            Position rx_m = reflectPoint(env.rx, segment);

            // no valid reflection possible if d(Tx, Rx) > d(Tx, Rx')
            if (squaredLengthTxRx > squaredLength(env.tx, rx_m)) {
                // skip segment because Tx and Rx are on opposite sides of segment
                continue;
            }

            // intersection between ray TxRx' and segment is reflection point
            PositionSegment ray(env.tx, rx_m);
            bg::intersection(segment, ray, intersections);
            if (intersections.size() == 1 && !isRayObstructed(intersections[0], env)) {
                rays.emplace_back(intersections[0]);
            }
            intersections.clear();
        }
    }

    return rays;
}

bool NLOSb::isRayObstructed(const Position& point, const Environment& env) const
{
    const PositionSegment seg_tx { env.tx, point };
    const PositionSegment seg_rx { point, env.rx };

    for (const ObstacleIndex::Obstacle* obstacle : env.obstacles)
    {
        if (bg::intersects(seg_tx, obstacle->getOutline())) {
            return true;
        } else if (bg::intersects(seg_rx, obstacle->getOutline())) {
            return true;
        }
    }

    for (const VehicleIndex::Vehicle* vehicle : env.vehicles)
    {
        if (bg::intersects(seg_tx, vehicle->getOutline())) {
            return true;
        } else if (bg::intersects(seg_rx, vehicle->getOutline())) {
            return true;
        }
    }

    return false;
}

std::vector<Position> NLOSb::computeDiffractionRays(const Environment& env) const
{
    std::vector<Position> corners;
    const double minVehicleHeight = std::min(env.txHeight.value(), env.rxHeight.value());

    auto obstacles = mObstacleIndex->getObstructingObstacles(env.tx, env.rx);
    for (auto& obstacle : obstacles)
    {
        for (auto& corner : obstacle->getOutline())
        {
            if (mObstacleIndex->anyBlockage(env.tx, corner)) {
                // TxC is blocked by a building
                continue;
            } else if (mObstacleIndex->anyBlockage(corner, env.rx)) {
                // CRx is blocked by a building
                continue;
            } else if (mVehicleIndex->anyBlockage(env.tx, corner, minVehicleHeight)) {
                // TxC is blocked by a tall enough vehicle
                continue;
            } else if (mVehicleIndex->anyBlockage(corner, env.rx, minVehicleHeight)) {
                // CRx is blocked by a tall enough vehicle
                continue;
            }

            corners.emplace_back(corner);
        }
    }

    if (mVisualizer) {
        mVisualizer->drawDiffractionRays(env.tx, env.rx, corners);
    }

    return corners;
}

std::vector<NLOSb::Attenuation> NLOSb::computeDiffractionAttenuation(const std::vector<Position>& corners, Length limit, const Environment& env) const
{
    std::vector<Attenuation> attenuations;
    const inet::m rx { env.rx.x / boost::units::si::meter };
    const inet::m ry { env.rx.y / boost::units::si::meter };
    const inet::m tx { env.tx.x / boost::units::si::meter };
    const inet::m ty { env.tx.y / boost::units::si::meter };
    const inet::m vx = rx - tx;
    const inet::m vy = ry - ty;
    const auto vlen_squared = squared(vx) + squared(vy);
    const inet::m vlen = sqrt(vlen_squared);
    const auto diffTxRx_squared = squared(env.txHeight - env.rxHeight);
    const double freeSpaceLoss = computeFreeSpaceLoss(vlen, env.lambda);

    for (const Position& corner : corners)
    {
        const Length dist = sqrt(squared(distance(env.tx, corner) + distance(env.rx, corner)) + diffTxRx_squared);
        if (dist > limit) {
            // reject diffraction rays longer than limit (usually limit is shortest reflection ray)
            continue;
        }

        const inet::m cx { corner.x / boost::units::si::meter };
        const inet::m cy { corner.y / boost::units::si::meter };
        const double k = inet::unit { (cx * vx - vy * ty + vy * cy - tx * vx) / vlen_squared }.get();
        if (k < 0.0 || k > 1.0) {
            // this particular corner is not suitable for diffraction
            continue;
        }
        const double l = inet::unit { (cx * vy - vy * tx - vx * cy + ty * vx) / vlen_squared }.get();
        const inet::m d = std::abs(k) * vlen;
        const inet::m h = std::abs(l) * vlen;

        double attenuation = computeSimpleKnifeEdge(h, d, vlen, env.lambda);
        attenuation /= freeSpaceLoss;
        attenuations.emplace_back(dist, attenuation);
    }

    return attenuations;
}

std::vector<NLOSb::Attenuation> NLOSb::computeReflectionAttenuation(const std::vector<Position>& points, double permittivity, const Environment& env) const
{
    std::vector<Attenuation> attenuation;
    const auto diffTxRx_squared = squared(env.txHeight - env.rxHeight);

    for (const Position& point : points)
    {
        // NOTE: theta is angle between reflecting surface and incident ray (see Rappaport)
        const double theta = (M_PI - computeAngleBetween(point, env.tx, env.rx)) / 2.0;
        const double gamma = computeReflectionCoefficient(theta, permittivity);

        Length dist = sqrt(squared(distance(env.tx, point) + distance(env.rx, point)) + diffTxRx_squared);
        double loss = squared(gamma) * computeFreeSpaceLoss(inet::m { dist / boost::units::si::meter }, env.lambda);
        attenuation.emplace_back(dist, 1.0 / loss);
    }

    return attenuation;
}

double NLOSb::combineAttenuations(const std::vector<Attenuation>& atts, inet::m lambda) const
{
    double loss = 0.0; // no signal
    if (atts.size() > 1) {
        auto minAtt = std::min_element(atts.begin(), atts.end(),
                [](const Attenuation& a, const Attenuation& b) {
                    return a.fraction < b.fraction;
                });
        const auto refDist = minAtt->length;
        const Length waveLength = lambda.get() * boost::units::si::meter;

        for (const Attenuation& att : atts)
        {
            const double phi = std::cos(2.0 * M_PI * (refDist - att.length) / waveLength);
            loss += std::sqrt(1.0 / att.fraction) * phi;
        }

        loss = squared(loss);
    } else if (!atts.empty()) {
        loss = 1.0 / atts[0].fraction;
    }

    ASSERT(loss >= 0.0 && loss <= 1.0);
    return loss;
}

double NLOSb::computeFreeSpaceLoss(inet::m dist, inet::m lambda) const
{
    if (dist.get() > 0.0) {
        // use fixed path loss exponent of 2 here as does the original GEMV2 implementation
        return inet::unit { squared(lambda) / squared(4.0 * M_PI * dist) }.get();
    } else {
        return 1.0;
    }
}

double NLOSb::computeLogDistanceLoss(inet::m dist, inet::m lambda) const
{
    // in the end log-distance is identical to free-space loss (using same path loss exponent)
    if (dist.get() > 0.0) {
        return squared(lambda.get()) / (squared(4.0 * M_PI) * pow(dist.get(), pathLossExponent));
    } else {
        return 1.0;
    }
}

double NLOSb::computeAngleBetween(const Position& nadir, const Position& a, const Position& b) const
{
    using Length = Position::value_type;
    const Length ax = a.x - nadir.x;
    const Length ay = a.y - nadir.y;
    const Length bx = b.x - nadir.x;
    const Length by = b.y - nadir.y;

    const auto dot_product = ax * bx + ay * by;
    const Length alen = sqrt(squared(ax) + squared(ay));
    const Length blen = sqrt(squared(bx) + squared(by));

    static const Length zero = Length::from_value(0.0);
    return (blen > zero && alen > zero) ? std::acos(dot_product / (alen * blen)) : 0.0;
}

double NLOSb::computeReflectionCoefficient(double theta, double permittivity) const
{
    double gamma = 0.0;

    if (polarization == 'v') {
        // E-field is normal to plane of incidence (see Equation 4.25 in Rappaport)
        const double sqrt_term = sqrt(permittivity - squared(cos(theta)));
        const double sin_theta = sin(theta);
        gamma = (sin_theta - sqrt_term) / (sin_theta + sqrt_term);
    } else if (polarization == 'h') {
        // E-field is in plane of incidence (see Equationo 4.24 in Rapport)
        const double sqrt_term = sqrt(permittivity - squared(cos(theta)));
        const double sin_theta = sin(theta);
        gamma = (-permittivity * sin_theta + sqrt_term) / (permittivity * sin_theta + sqrt_term);
    } else {
        error("Unknown antenna polarization");
    }

    ASSERT(std::abs(gamma) <= 1.0);
    return gamma;
}

double NLOSb::computeSimpleKnifeEdge(m h, m distTxObs, m distTxRx, m lambda) const
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

    const m distRxObs = distTxRx - distTxObs;
    // calculate the Fresnel ray
    const m r = sqrt(lambda * distTxObs * distRxObs / distTxRx);
    static const double root_two = sqrt(2.0);
    const double v = root_two * static_cast<inet::unit>(h / r).get();

    double attenuation = 0.0;
    if (v > -0.78) {
        // approximation of Fresnel-Kirchoff loss given by ITU-R P.526, equation 31 (result in dB):
        // J(v) = 6.9 + 20 log(sqrt((v - 01)^2 + 1) + v - 0.1)
        attenuation = 6.9 + 20.0 * log10(sqrt(squared(v - 0.1) + 1.0) + v - 0.1);
    }

    return inet::math::dB2fraction(attenuation);
}

NLOSb::Environment::Environment(const NLOSb* model, const inet::Coord& itx, const inet::Coord& irx, inet::m lambda) :
    tx(itx.x, itx.y), rx(irx.x, irx.y),
    txHeight(itx.z * boost::units::si::meter), rxHeight(irx.z * boost::units::si::meter),
    obstacles(model->mObstacleIndex->obstaclesEllipse(tx, rx, model->maxRange)),
    vehicles(model->mVehicleIndex->vehiclesEllipseOthers(tx, rx, model->maxRange)),
    lambda(lambda)
{
}

NLOSb::Attenuation::Attenuation(Length len, double att) :
    length(len), fraction(att)
{
    ASSERT(fraction > 1.0);
}

} // namespace gemv2
} // namespace artery
