/*
 * Artery V2X Simulation Framework
 * Copyright 2018 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_GEMV2_NLOSF_H_WLNRGJIS
#define ARTERY_GEMV2_NLOSF_H_WLNRGJIS

#include "artery/inet/gemv2/Math.h"
#include "artery/utility/Geometry.h"
#include <boost/geometry/geometries/linestring.hpp>
#include <inet/physicallayer/pathloss/FreeSpacePathLoss.h>

namespace artery
{
namespace gemv2
{

// forward declarations
class ObstacleIndex;
class Visualizer;

class NLOSf : public inet::physicallayer::FreeSpacePathLoss
{
public:
    void initialize(int stage) override;
    std::ostream& printToStream(std::ostream& stream, int level) const override;
    double computePathLoss(const inet::physicallayer::ITransmission *transmission, const inet::physicallayer::IArrival *arrival) const override;
    double computePathLoss(inet::mps propagation, inet::Hz frequency, inet::m distance) const override;

protected:
    using PositionLineString = boost::geometry::model::linestring<Position>;

    inet::m computeDistanceThroughFoliage(const inet::Coord& a, const inet::Coord& b) const;
    inet::m mergeIntervals(std::list<PositionLineString>&, const Position&) const;

    const ObstacleIndex* mFoliageIndex = nullptr;
    Visualizer* mVisualizer = nullptr;
};

} // namespace gemv2
} // namespace artery

#endif /* ARTERY_GEMV2_NLOSF_H_WLNRGJIS */
