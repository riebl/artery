/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ENVIRONMENTMODELOBSTACLE_H_
#define ENVIRONMENTMODELOBSTACLE_H_

#include "artery/envmod/EnvironmentModelObject.h"
#include "artery/utility/Geometry.h"
#include <boost/geometry/index/rtree.hpp>
#include <string>
#include <vector>

namespace artery
{

/**
 * Representation of an obstacle inside the environment model
 */
class EnvironmentModelObstacle
{
public:
    EnvironmentModelObstacle(std::string id, std::vector<Position> outline) :
        mId(id), mPolygon(outline) {}

    /**
     * Returns the obstacle id
     * @return obstacle id
     */
    const std::string& getObstacleId() const { return mId; }

    /**
     * Returns the obstacle coords
     * @return obstacle coords
     */
    const std::vector<Position>& getOutline() const { return mPolygon; }

private:
    std::string mId; //!< Unique obstacle id
    std::vector<Position> mPolygon; //!< Obstacle outline
};

using ObstacleDB = std::map<std::string, std::shared_ptr<EnvironmentModelObstacle>>;
using ObstacleRtreeValue = std::pair<geometry::Box, std::string>;
using ObstacleRtree = boost::geometry::index::rtree<ObstacleRtreeValue, boost::geometry::index::rstar<16>>;

} // namespace artery

#endif /* ENVIRONMENTMODELOBSTACLE_H_ */
