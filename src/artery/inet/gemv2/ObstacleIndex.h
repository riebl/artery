/*
 * Artery V2X Simulation Framework
 * Copyright 2017 Thiago Vieira, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef OBSTACLEINDEX_H_WKZBN6QH
#define OBSTACLEINDEX_H_WKZBN6QH

#include "artery/utility/Geometry.h"
#include <boost/geometry/index/rtree.hpp>
#include <omnetpp/clistener.h>
#include <omnetpp/csimplemodule.h>
#include <set>
#include <string>
#include <vector>

// forward declaration
namespace traci { class LiteAPI; }

namespace artery
{
namespace gemv2
{

class ObstacleIndex : public omnetpp::cSimpleModule, public omnetpp::cListener
{
public:
    // cSimpleModule
    void initialize() override;

    // cListener
    void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, const omnetpp::SimTime&, omnetpp::cObject*) override;

    bool anyBlockage(const Position& a, const Position& b) const;

private:
    void fetchObstacles(traci::LiteAPI&);

    class Obstacle
    {
    public:
        Obstacle(std::vector<Position>&& shape);
        const std::vector<Position>& getOutline() const { return mOutline; }
        double getArea() const { return mArea; }
        const Position& getCentroid() const { return mCentroid; }

    private:
        std::vector<Position> mOutline;
        Position mCentroid;
        double mArea;
    };

    using RtreeValue = std::pair<geometry::Box, std::size_t>;
    using Rtree = boost::geometry::index::rtree<RtreeValue, boost::geometry::index::rstar<16>>;

    std::set<std::string> mFilterTypes;
    std::vector<Obstacle> mObstacles;
    Rtree mObstacleRtree;
};

} // namespace gemv2
} // namespace artery

#endif /* OBSTACLEINDEX_H_WKZBN6QH */

