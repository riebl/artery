/*
 * Artery V2X Simulation Framework
 * Copyright 2017 Thiago Vieira, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef SMALL_SCALE_H
#define SMALL_SCALE_H

#include "artery/inet/gemv2/LinkClass.h"
#include "inet/common/Units.h"
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

class SmallScaleVariation : public omnetpp::cSimpleModule
{
public:
   using m = inet::units::values::m;

   void initialize() override;
   void finish() override;

   double computeVariation(const Position& a, const Position& b, m range, LinkClass link) const;
   double computeVariation(const Position& a, const Position& b, m range, double minSD, double maxSD) const;

private:
   const ObstacleIndex* mObstacleIndex;
   const VehicleIndex* mVehicleIndex;

   double mMaxVehicleDensity;
   double mMaxObstacleDensity;
   mutable double mMaxObservedVehicleDensity;
   mutable double mMaxObservedObstacleDensity;

   // Minimum stddev of small scale variation for LOS and NLOSv / b links
   double mMinStdDevLOS;
   double mMinStdDevNLOSv;
   double mMinStdDevNLOSb;
   double mMinStdDevNLOSf;

   // Maximum stddev of small scale variation for LOS and NLOSv / b links
   double mMaxStdDevLOS;
   double mMaxStdDevNLOSv;
   double mMaxStdDevNLOSb;
   double mMaxStdDevNLOSf;
};

} // namespace gemv2
} // namespace artery

#endif /* SMALL_SCALE_H */
