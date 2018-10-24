/*
* Artery V2X Simulation Framework
* Copyright 2018 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/inet/gemv2/NLOSf.h"
#include "artery/inet/gemv2/Math.h"
#include "artery/inet/gemv2/ObstacleIndex.h"
#include "artery/inet/gemv2/Visualizer.h"
#include <inet/common/ModuleAccess.h>
#include <inet/common/Units.h>
#include <inet/physicallayer/contract/packetlevel/IRadioMedium.h>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/register/linestring.hpp>
#include <algorithm>
#include <array>
#include <iterator>
#include <list>

using namespace inet;

namespace { using PositionSegment = std::array<artery::Position, 2>; }
BOOST_GEOMETRY_REGISTER_LINESTRING(PositionSegment)

namespace artery
{
namespace gemv2
{

Define_Module(NLOSf)

void NLOSf::initialize(int stage)
{
    FreeSpacePathLoss::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        mFoliageIndex = inet::findModuleFromPar<ObstacleIndex>(par("foliageIndexModule"), this);
        mVisualizer = inet::findModuleFromPar<Visualizer>(par("visualizerModule"), this, false);
    }
}

std::ostream& NLOSf::printToStream(std::ostream& stream, int level) const
{
    stream << "GEMV2 NLOSf";
    if (level <= PRINT_LEVEL_TRACE)
        stream << ", alpha = " << alpha
               << ", systemLoss = " << systemLoss;
    return stream;
}

double NLOSf::computePathLoss(const physicallayer::ITransmission *transmission, const physicallayer::IArrival *arrival) const
{
    auto radioMedium = transmission->getTransmitter()->getMedium();
    auto narrowbandSignalAnalogModel = check_and_cast<const physicallayer::INarrowbandSignal *>(transmission->getAnalogModel());
    const mps propagationSpeed = radioMedium->getPropagation()->getPropagationSpeed();
    const Hz carrierFrequency = narrowbandSignalAnalogModel->getCarrierFrequency();
    const m waveLength = propagationSpeed / carrierFrequency;
    const m dist { transmission->getStartPosition().distance(arrival->getStartPosition()) };

    const double attenuationPerMeter_dB = 0.79 * std::pow(GHz(carrierFrequency).get(), 0.61);
    const inet::m foliageDist = computeDistanceThroughFoliage(transmission->getStartPosition(), arrival->getStartPosition());
    const double foliageAttenuation_dB = attenuationPerMeter_dB * foliageDist.get();
    const double freeSpaceLoss = computeFreeSpacePathLoss(waveLength, dist, alpha, systemLoss);
    return freeSpaceLoss / math::dB2fraction(foliageAttenuation_dB);
}

double NLOSf::computePathLoss(mps propagationSpeed, Hz carrierFrequency, m distance) const
{
    // insufficient input data, need LOS path for loss computation
    return NaN;
}

m NLOSf::computeDistanceThroughFoliage(const Coord& tx, const Coord& rx) const
{
    const Position start { tx.x, tx.y };
    const Position end { rx.x, rx.y };
    const PositionSegment ray { start, end };

    std::list<PositionLineString> intersections;
    const auto plants = mFoliageIndex->getObstructingObstacles(start, end);
    for (const ObstacleIndex::Obstacle* plant : plants)
    {
        boost::geometry::intersection(ray, plant->getOutline(), intersections);
    }
    inet::m dist = mergeIntervals(intersections, start);

    if (mVisualizer) {
        std::vector<Position> visualization;
        visualization.reserve(intersections.size() * 2);
        for (const PositionLineString& ls : intersections)
        {
            std::copy(ls.begin(), ls.end(), std::back_inserter(visualization));
        }
        mVisualizer->drawFoliageRay(start, end, visualization);
    }

    return dist;
}

inet::m NLOSf::mergeIntervals(std::list<PositionLineString>& lines, const Position& ref) const
{
    using LineIterator = std::list<PositionLineString>::iterator;
    struct OrderedLineString
    {
        OrderedLineString(LineIterator it, double idxStart, double idxStop) :
            line(it), indexStart(idxStart), indexStop(idxStop) { ASSERT(indexStart <= indexStop); }
        bool operator<(const OrderedLineString& other) const { return this->indexStart < other.indexStart; }

        LineIterator line;
        double indexStart;
        double indexStop;
    };

    // there is nothing to merge so bail out quickly
    if (lines.empty()) {
        return inet::m { 0.0 };
    } else if (lines.size() == 1) {
        return inet::m { static_cast<double>(boost::geometry::length(lines.front())) };
    }

    auto calculateIndex = [&ref](const Position& pos) -> double {
        return (squared(pos.x - ref.x) + squared(pos.y - ref.y)).value();
    };

    // create index entry for each line segment
    std::list<OrderedLineString> ordered;
    for (auto it = lines.begin(); it != lines.end();)
    {
        if (it->size() != 2) {
            it = lines.erase(it);
            continue;
        }

        const double index1 = calculateIndex((*it)[0]);
        const double index2 = calculateIndex((*it)[1]);
        if (index1 < index2) {
            ordered.emplace_back(it, index1, index2);
        } else {
            boost::geometry::reverse(*it);
            ordered.emplace_back(it, index2, index1);
        }

        ++it;
    }
    ASSERT(ordered.size() == lines.size());
    ordered.sort();

    // move lines to backlog (some of them are moved back in new order later)
    std::list<PositionLineString> backlog;
    backlog.splice(backlog.end(), lines);
    ASSERT(lines.size() == 0);

    // merge overlapping segments and calculate total distance through foliage
    inet::m dist { 0.0 };
    auto current = ordered.begin();
    lines.splice(lines.end(), backlog, current->line);
    for (auto it = std::next(current); it != ordered.end(); ++it)
    {
        if (current->indexStop < it->indexStart) {
            // disjoint intervals: add interval to output
            dist += inet::m { std::sqrt(current->indexStop) - std::sqrt(current->indexStart) };
            current = it;
            lines.splice(lines.end(), backlog, current->line);
        } else if (current->indexStop < it->indexStop) {
            // overlapping intervals: adjust stop of first and remove other
            current->line->back() = it->line->back();
            current->indexStop = it->indexStop;
        }
        // else: interval completely covered by current (no op)
    }
    dist += inet::m { std::sqrt(current->indexStop) - std::sqrt(current->indexStart) };

    return dist;
}

} // namespace gemv2
} // namespace artery
