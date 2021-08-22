/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef SENSORDETECTION_H_TLUSLFDM
#define SENSORDETECTION_H_TLUSLFDM

#include "artery/envmod/EnvironmentModelObject.h"
#include "artery/envmod/EnvironmentModelObstacle.h"
#include "artery/utility/Geometry.h"
#include <list>
#include <memory>
#include <vector>

namespace artery
{

struct SensorDetection
{
    Position sensorOrigin;
    std::vector<Position> sensorCone;
    std::list<std::shared_ptr<EnvironmentModelObject>> objects;
    std::list<std::shared_ptr<EnvironmentModelObstacle>> obstacles;
    std::list<Position> visiblePoints; // LOS = one of these points and first of sensorCone
};

} // namespace artery

#endif /* SENSORDETECTION_H_TLUSLFDM */

