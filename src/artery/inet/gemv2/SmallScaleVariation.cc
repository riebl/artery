/*
* Artery V2X Simulation Framework
* Copyright 2017 Thiago Vieira, Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include <artery/inet/gemv2/Math.h>
#include <artery/inet/gemv2/ObstacleIndex.h>
#include <artery/inet/gemv2/SmallScaleVariation.h>
#include <artery/inet/gemv2/VehicleIndex.h>
#include <inet/common/INETMath.h>
#include <inet/common/ModuleAccess.h>
#include <cmath>
#include <limits>
#include <numeric>
#include <vector>

namespace artery
{
namespace gemv2
{

Define_Module(SmallScaleVariation)

void SmallScaleVariation::initialize()
{
    mObstacleIndex = inet::findModuleFromPar<ObstacleIndex>(par("obstacleIndexModule"), this);
    mVehicleIndex = inet::findModuleFromPar<VehicleIndex>(par("vehicleIndexModule"), this);

    // Minimum stddev of small scale variation
    mMinStdDevLOS = par("minStdDevLOS");
    mMinStdDevNLOSv = par("minStdDevNLOSv");
    mMinStdDevNLOSb = par("minStdDevNLOSb");
    mMinStdDevNLOSf = par("minStdDevNLOSf");

    // Maximum stddev of small scale variation
    mMaxStdDevLOS = par("maxStdDevLOS");
    mMaxStdDevNLOSv = par("maxStdDevNLOSv");
    mMaxStdDevNLOSb = par("maxStdDevNLOSb");
    mMaxStdDevNLOSf = par("maxStdDevNLOSf");

    mMaxVehicleDensity = par("maxVehicleDensity");
    mMaxObstacleDensity = par("maxObstacleDensity");
    mMaxObservedVehicleDensity = -std::numeric_limits<double>::infinity();
    mMaxObservedObstacleDensity = -std::numeric_limits<double>::infinity();
    WATCH(mMaxObservedVehicleDensity);
    WATCH(mMaxObservedObstacleDensity);
}

void SmallScaleVariation::finish()
{
    EV_INFO << "maximum observed vehicle density: " << mMaxObservedVehicleDensity << "\n";
    if (mMaxObservedVehicleDensity > mMaxVehicleDensity) {
        EV_WARN << "update maximum vehicle density in configuration!\n";
    }
    recordScalar("maxVehicleDensity", mMaxObservedVehicleDensity);

    EV_INFO << "maximum observed obstacle density: " << mMaxObservedObstacleDensity << "\n";
    if (mMaxObservedObstacleDensity > mMaxObstacleDensity) {
        EV_WARN << "update maximum obstacle density in configuration!\n";
    }
    recordScalar("maxObstacleDensity", mMaxObservedObstacleDensity);
}

double SmallScaleVariation::computeVariation(const Position& a, const Position& b, m range, LinkClass link) const
{
    double minDev = 0.0;
    double maxDev = 0.0;

    switch (link) {
        case LinkClass::LOS:
            minDev = mMinStdDevLOS;
            maxDev = mMaxStdDevLOS;
            break;
        case LinkClass::NLOSv:
            minDev = mMinStdDevNLOSv;
            maxDev = mMaxStdDevNLOSv;
            break;
        case LinkClass::NLOSb:
            minDev = mMinStdDevNLOSb;
            maxDev = mMaxStdDevNLOSb;
            break;
        case LinkClass::NLOSf:
            minDev = mMinStdDevNLOSf;
            maxDev = mMaxStdDevNLOSf;
            break;
        default:
            EV_ERROR << "Unknown link class, falling back to zero deviation\n";
            break;
    }

    return computeVariation(a, b, range, minDev, maxDev);
}

double SmallScaleVariation::computeVariation(const Position& a, const Position& b, m range, double minDev, double maxDev) const
{
    using Obstacle = ObstacleIndex::Obstacle;
    using Vehicle = VehicleIndex::Vehicle;
    std::vector<const Obstacle*> obstacles = mObstacleIndex->obstaclesEllipse(a, b, range.get());
    std::vector<const Vehicle*> vehicles = mVehicleIndex->vehiclesEllipse(a, b, range.get());

    // Calculate relative vehicle density: number of vehicles divided by squared effective range
    const double relVehDensity = vehicles.size() / squared(range.get());
    if (relVehDensity > mMaxObservedVehicleDensity) {
        mMaxObservedVehicleDensity = relVehDensity;
    }

    // Calculate relative obstacle density: area covered by obstacles divided by squared range
    const double obsTotalArea = std::accumulate(obstacles.begin(), obstacles.end(), 0.0,
            [](double accu, const Obstacle* obs) {
                return accu + obs->getArea();
            });
    const double relObsDensity = obsTotalArea / squared(range.get());
    if (relObsDensity > mMaxObservedObstacleDensity) {
        mMaxObservedObstacleDensity = relObsDensity;
    }

    // Calculate the vehicle density coefficient and static density coefficient
    const double vehDensityCoeff = std::min(1.0, sqrt(relVehDensity / mMaxVehicleDensity));
    const double obsDensityCoeff = std::min(1.0, sqrt(relObsDensity / mMaxObstacleDensity));
    const double deviation = minDev + 0.5 * (maxDev - minDev) * (vehDensityCoeff + obsDensityCoeff);
    return inet::math::dB2fraction(normal(0.0, deviation));
}

} // namespace gemv2
} // namespace artery
