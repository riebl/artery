/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_ENVMOD_PRESELECTIONMETHOD_H_
#define ARTERY_ENVMOD_PRESELECTIONMETHOD_H_

#include "artery/envmod/GlobalEnvironmentModel.h"
#include <string>
#include <vector>

namespace artery
{

class EnvironmentModelObject;
class SensorConfigRadar;

class PreselectionMethod
{
public:
    using Objects = GlobalEnvironmentModel::ObjectDB;

    PreselectionMethod(const GlobalEnvironmentModel::ObjectDB& objs) : mObjects(objs) {}
    virtual void update() = 0;
    virtual std::vector<std::string> select(const EnvironmentModelObject& ego, const SensorConfigRadar&) = 0;

protected:
    const Objects& mObjects;
};

} // namespace artery

#endif /* ARTERY_ENVMOD_PRESELECTIONMETHOD_H_ */

