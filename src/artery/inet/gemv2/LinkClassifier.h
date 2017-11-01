/*
 * Artery V2X Simulation Framework
 * Copyright 2017 Thiago Vieira, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef LINKCLASSIFIER_H_OAXCBN1T
#define LINKCLASSIFIER_H_OAXCBN1T

#include <omnetpp/csimplemodule.h>

// forward declaration
class Position;

namespace artery
{
namespace gemv2
{

// forward declarations
class ObstacleIndex;
class VehicleIndex;

class LinkClassifier : public omnetpp::cSimpleModule
{
public:
    enum class Link { LOS, NLOSb, NLOSv };

    void initialize() override;
    Link classifyLink(const Position& tx, const Position& rx) const;

private:
    const ObstacleIndex* mObstacleIndex;
    const VehicleIndex* mVehicleIndex;
};

} // namespace gemv2
} // namespace artery

#endif /* LINKCLASSIFIER_H_OAXCBN1T */

