/*
* Artery V2X Simulation Framework
* Copyright 2017, 2018 Thiago Vieira, Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_GEMV2_NLOSB_H_NV3WEACB
#define ARTERY_GEMV2_NLOSB_H_NV3WEACB

#include "artery/inet/gemv2/ObstacleIndex.h"
#include "artery/inet/gemv2/VehicleIndex.h"
#include <inet/common/ModuleAccess.h>
#include <inet/common/Units.h>
#include <inet/physicallayer/contract/packetlevel/IPathLoss.h>
#include <omnetpp/csimplemodule.h>
#include <vector>

namespace artery
{
namespace gemv2
{

class Visualizer;

class NLOSb : public omnetpp::cSimpleModule, public inet::physicallayer::IPathLoss
{
public:
    NLOSb();
    void initialize() override;
    double computePathLoss(const inet::physicallayer::ITransmission*, const inet::physicallayer::IArrival*) const override;
    double computePathLoss(inet::mps propagation, inet::Hz frequency, inet::m distance) const override;
    inet::m computeRange(inet::mps propagation, inet::Hz frequency, double loss) const override;

protected:
    using VehicleList = std::vector<const VehicleIndex::Vehicle*>;
    using ObstacleList = std::vector<const ObstacleIndex::Obstacle*>;
    using Length = Position::value_type;

    /**
     * Attenuation represents the signal attenuation along a particular propagation path
     */
    struct Attenuation
    {
        Attenuation(Length l, double f);

        Length length;
        double fraction;
    };

    /**
     * Environment stores attributes commonly used by NLOSb methods representing the specific propagation environment,
     * i.e. depending on a particular transmission
     */
    struct Environment
    {
        Environment(const NLOSb*, const inet::Coord& tx, const inet::Coord& rx, inet::m lambda);

        Position tx;
        Position rx;
        Length txHeight;
        Length rxHeight;
        ObstacleList obstacles;
        VehicleList vehicles; /*< except transmitter and receiver */
        inet::m lambda;
    };
    friend struct Environment;

    /**
     * Compute reflection rays based on single interaction with builidings
     * \param env NLOSb environment
     * \return points at buildings where signal (env) can be reflected without obstruction
     */
    virtual std::vector<Position> computeReflectionRaysFromBuildings(const Environment& env) const;

    /**
     * Compute reflection rays based on single interaction with vehicles
     * \param env NLOSb environment
     * \return points at vehicles where signal (env) can be reflected without obstruction
     */
    virtual std::vector<Position> computeReflectionRaysFromVehicles(const Environment& env) const;

    /**
     * Compute diffraction rays based on single interaction with corners of buildings
     * \param env NLOSb environment
     * \return corners of buildings where diffraction without obstruction can happen
     */
    virtual std::vector<Position> computeDiffractionRays(const Environment& env) const;

    /**
     * Check if a given ray is obstructed by any building or vehicle
     * \param p point P, signal ray goes from Tx to P to Rx
     * \param env contains Tx and Rx positions
     */
    virtual bool isRayObstructed(const Position& p, const Environment& env) const;

    /**
     * Compute attenuation along given reflection paths
     * \param refls reflection paths (list of reflection points)
     * \param permittivity Relative permittivity of surface at reflection points
     * \param env NLOS environment
     * \return attenuation for each given reflection path
     */
    virtual std::vector<Attenuation> computeReflectionAttenuation(const std::vector<Position>& refls, double permittivity, const Environment&) const;

    /**
     * Compute attenuation along given diffraction paths
     * \param corners corner points at which diffraction occurs
     * \param limit reject diffraction paths exceedings this length limit
     * \param env NLOSb environment
     * \return attenuation for each feasible diffraction path
     */
    virtual std::vector<Attenuation> computeDiffractionAttenuation(const std::vector<Position>& corners, Length limit, const Environment&) const;

    /**
     * Combine attenuation paths to a final signal loss factor
     * \param atts attenuation paths
     * \param lambda wave length of signal
     * \return signal loss factor (inverse of summed attenuation)
     */
    virtual double combineAttenuations(const std::vector<Attenuation>& atts, inet::m lambda) const;

    /**
     * Calculate loss based on free-space model
     * \param dist length of signal path
     * \param lambda wave length of signal
     * \return signal loss factor (inverse of attenuation)
     */
    virtual double computeFreeSpaceLoss(inet::m dist, inet::m lambda) const;

    /**
     * Calculate loss based on log-distance model
     * \param dist length of signal path
     * \param lambda wave length of signal
     * \return signal loss factor (inverse of attenuation)
     */
    virtual double computeLogDistanceLoss(inet::m dist, inet::m lambda) const;

    /**
     * Calculate attenuation based on simple knife-edge model
     * \param heightObs height of obstacle ("knife-edge")
     * \param distTxObs distance between Tx and obstacle on direct line-of-sight
     * \param distTxRx distance between Tx and Rx (line-of-sight)
     * \param lambda wave length
     * \return attenuation as fraction
     */
    double computeSimpleKnifeEdge(inet::m heightObs, inet::m distTxObs, inet::m distTxRx, inet::m lambda) const;

    /**
     * Compute reflection coefficient (Gamma)
     * This method depends on polarization member attribute!
     * \param theta incident angle (rad) between surface and ray
     * \param permittivity relative permittivity of surface
     * \return reflection coefficient
     */
    double computeReflectionCoefficient(double theta, double permittivity) const;

    /**
     * Compute angle between two vectors with common foot point
     * \param nadir common foot point
     * \param a vector a-> is pointing from nadir to a
     * \param b vector b-> is pointing from nadir to b
     * \return angle (rad) between a-> and b->
     */
    double computeAngleBetween(const Position& nadir, const Position& a, const Position& b) const;

    const VehicleIndex* mVehicleIndex;
    const ObstacleIndex* mObstacleIndex;

private:
    double maxRange;
    double pathLossExponent;
    double vehReflRelPerm; // Relative permittivity of vehicles
    double obsReflRelPerm; // Relative permittivity of buildings
    char polarization;
    Visualizer* mVisualizer = nullptr;
};

} // namespace gemv2
} // namespace artery

#endif //* ARTERY_GEMV2_NLOSB_H_NV3WEACB */
