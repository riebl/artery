/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_ENVMOD_PRESELECTIONMETHOD_H_
#define ARTERY_ENVMOD_PRESELECTIONMETHOD_H_

#include "artery/envmod/EnvironmentModelObstacle.h"
#include "artery/envmod/sensor/SensorConfiguration.h"
#include <string>
#include <vector>

namespace artery
{

class EnvironmentModelObject;
class SensorConfigFov;

class PreselectionMethod
{
public:
    using Objects = ObjectDB;

    PreselectionMethod(const ObjectDB& objs) : mObjects(objs) {}
    virtual void update() = 0;
    virtual std::vector<std::string> select(const EnvironmentModelObject& ego, const SensorConfigFov&) = 0;

protected:
    const Objects& mObjects;
};

} // namespace artery

#endif /* ARTERY_ENVMOD_PRESELECTIONMETHOD_H_ */

