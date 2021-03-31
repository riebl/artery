/*
 * Artery V2X Simulation Framework
 * Copyright 2017 Thiago Vieira, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef OBSTACLEINDEX_H_WKZBN6QH
#define OBSTACLEINDEX_H_WKZBN6QH

#include "artery/utility/Geometry.h"
#include <boost/geometry/index/rtree.hpp>
#include <omnetpp/ccanvas.h>
#include <omnetpp/clistener.h>
#include <omnetpp/csimplemodule.h>
#include <set>
#include <string>
#include <vector>

// forward declaration
namespace traci { class API; }

namespace artery
{
namespace gemv2
{

class Visualizer;

class ObstacleIndex : public omnetpp::cSimpleModule, public omnetpp::cListener
{
public:
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

    // cSimpleModule
    void initialize() override;

    // cListener
    void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, const omnetpp::SimTime&, omnetpp::cObject*) override;

    bool anyBlockage(const Position& a, const Position& b) const;

    /**
     * Get obstacles with their center point being within the defined ellipse.
     *
     * The foci of the ellipse are a and b, respectively.
     * Semi-major axis is 1/2 of given range.
     *
     * \return pointers to obstacles
     */
    std::vector<const Obstacle*> obstaclesEllipse(const Position& a, const Position& b, double range) const;

    /**
     * Get all obstacles obstructing the line of sight between given points
     * \param a position a, e.g. transmitter
     * \param b position b, e.g. receiver
     * \return pointer to obstacles obstructing line of sight
     */
    std::vector<const Obstacle*> getObstructingObstacles(const Position& a, const Position& b) const;

    /**
     * Get all currently indexed obstacles
     * \return obstacles
     */
    const std::vector<Obstacle>& getObstacles() const { return mObstacles; }

    /**
     * Get default color for drawing obstacles
     * \return color
     */
    omnetpp::cFigure::Color getColor() const { return mColor; }

private:
    void fetchObstacles(const traci::API&);

    using RtreeValue = std::pair<geometry::Box, std::size_t>;
    using Rtree = boost::geometry::index::rtree<RtreeValue, boost::geometry::index::rstar<16>>;

    std::set<std::string> mFilterTypes;
    std::vector<Obstacle> mObstacles;
    Rtree mObstacleRtree;
    Visualizer* mVisualizer = nullptr;
    omnetpp::cFigure::Color mColor;
};

} // namespace gemv2
} // namespace artery

#endif /* OBSTACLEINDEX_H_WKZBN6QH */

