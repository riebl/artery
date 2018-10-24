/*
 * Artery V2X Simulation Framework
 * Copyright 2017 Thiago Vieira, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef LINKCLASSIFIER_H_OAXCBN1T
#define LINKCLASSIFIER_H_OAXCBN1T

#include "LinkClass.h"
#include <omnetpp/csimplemodule.h>

namespace artery
{

// forward declaration
class Position;

namespace gemv2
{

// forward declarations
class ObstacleIndex;
class VehicleIndex;

class LinkClassifier : public omnetpp::cSimpleModule
{
public:
    void initialize() override;
    void finish() override;
    LinkClass classifyLink(const Position& tx, const Position& rx) const;

private:
    const ObstacleIndex* mObstacleIndex;
    const ObstacleIndex* mFoliageIndex;
    const VehicleIndex* mVehicleIndex;

    mutable unsigned mCountLOS = 0;
    mutable unsigned mCountNLOSb = 0;
    mutable unsigned mCountNLOSf = 0;
    mutable unsigned mCountNLOSv = 0;
};

} // namespace gemv2
} // namespace artery

#endif /* LINKCLASSIFIER_H_OAXCBN1T */

