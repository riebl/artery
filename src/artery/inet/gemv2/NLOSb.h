/*
* Artery V2X Simulation Framework
* Copyright 2017 Thiago Vieira, Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/


#ifndef ARTERY_GEMV2_NLOSB_H_NV3WEACB
#define ARTERY_GEMV2_NLOSB_H_NV3WEACB


#include <vector>
#include <artery/inet/gemv2/ObstacleIndex.h>
#include <inet/common/ModuleAccess.h>
#include <inet/physicallayer/contract/packetlevel/IPathLoss.h>
#include <artery/inet/gemv2/VehicleIndex.h>
#include <inet/common/Units.h>
#include <omnetpp/csimplemodule.h>
#include <list>

// forward declaration
//class Position;

namespace artery {
namespace gemv2 {
    
using meter = inet::units::values::m;


class NLOSb : public omnetpp::cSimpleModule, public inet::physicallayer::IPathLoss//, public VehicleIndex, public ObstacleIndex
{

public:
    NLOSb();
    void initialize() override;
    double computePathLoss(const inet::physicallayer::ITransmission*, const inet::physicallayer::IArrival*) const override;
    double computePathLoss(inet::mps propagation, inet::Hz frequency, inet::m distance) const override;
    meter computeRange(inet::mps propagation, inet::Hz frequency, double loss) const override;
   
protected:
    
    using VehicleList = std::vector<const VehicleIndex::Vehicle*>;
    using ObstacleList = std::vector<const ObstacleIndex::Obstacle*>;
    
    const ObstacleIndex* mObstacleIndex;
    const VehicleIndex* mVehicleIndex;
        
    //it has created to return two kinda variables: efields and reflection ray distance (supposed to be used in diffraction method)
    struct returns{
         
         std::vector<double> Efield;
         std::vector<double> dif_ref_dist;
    };

virtual std::vector<Position> Build_reflection_rays(const ObstacleList& obstacle,const inet::Coord& tx, const inet::Coord& rx, double maxNLOSbRange) const;
virtual std::vector<Position> Veh_reflection_rays(const VehicleList& vehicles,const inet::Coord& tx, const inet::Coord& rx,double  maxNLOSbRange) const; 
virtual std::vector<Position> outer_check(const inet::Coord& tx, const inet::Coord& rx, std::vector <Position> RaysToTest) const;
virtual returns compute_reflection(const inet::Coord& tx, const inet::Coord& rx, const inet::m lamba, int polarization) const;

virtual std::vector<Position> diffraction_rays(const inet::Coord& tx, const inet::Coord& rx, std::vector<double> dif_ref_dist, double maxNLOSbRange) const;
virtual returns compute_diffraction (std::vector<Position> rays, const inet::Coord& tx, const inet::Coord& rx, inet::m lambda) const; 
virtual double computeSimpleKnifeEdgeCorner(double CornerHeight, double RealDistanceTxRx, double DistanceProjTx, inet::m lambda) const; 
    
private:
    double buildingReflRelPerm; //Relative permittivity of buildings
    double vehReflRelPerm; // Relative permittivity of vehicles
    double maxNLOSbRange;
};

} // namespace gemv2
} // namespace artery

#endif //* ARTERY_GEMV2_NLOSB_H_NV3WEACB */
