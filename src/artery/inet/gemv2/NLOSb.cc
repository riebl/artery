/*
* Artery V2X Simulation Framework
* Copyright 2017 Thiago Vieira, Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/inet/gemv2/NLOSb.h"
#include <artery/inet/gemv2/ObstacleIndex.h>
#include <inet/common/ModuleAccess.h>
#include <inet/physicallayer/contract/packetlevel/IPathLoss.h>
#include <inet/physicallayer/contract/packetlevel/IRadioMedium.h>
#include <artery/inet/gemv2/VehicleIndex.h>
#include <vector>
#include <iostream>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <cmath>
#include <boost/geometry/geometries/geometries.hpp> 
#include <algorithm>
#include <inet/common/Units.h>

namespace bg = boost::geometry;

// shortcut for squaring numbers (should be faster than pow(x, 2.0))
namespace { constexpr double squared(double x) { return x * x; } }

namespace artery
{
namespace gemv2
{

Define_Module(NLOSb);

using namespace inet;
namespace phy = inet::physicallayer;

void NLOSb::initialize()
{
    mObstacleIndex = inet::findModuleFromPar<ObstacleIndex>(par("obstacleIndexModule"), this);
    mVehicleIndex = inet::findModuleFromPar<VehicleIndex>(par("vehicleIndexModule"), this);
    
    maxNLOSbRange = par("maxRange").doubleValue();
    vehReflRelPerm = par("veh");
    buildingReflRelPerm = par("build");
}

NLOSb::NLOSb() :
    mVehicleIndex(nullptr), mObstacleIndex(nullptr),
    maxNLOSbRange(300.0), vehReflRelPerm(6.0), buildingReflRelPerm(4.5)
{
}

double NLOSb::computePathLoss(mps propagationSpeed, Hz frequency, m distance) const
{
    return NaN;
}

m NLOSb::computeRange(mps propagationSpeed, Hz frequency, double loss) const
{
    return m(NaN);
}

double NLOSb::computePathLoss(const phy::ITransmission* transmission, const phy::IArrival* arrival) const
{
    auto radioMedium = transmission->getTransmitter()->getMedium();
    auto narrowbandSignalAnalogModel = check_and_cast<const phy::INarrowbandSignal *>(transmission->getAnalogModel());
    const mps propagationSpeed = radioMedium->getPropagation()->getPropagationSpeed();
    const Hz carrierFrequency = Hz(narrowbandSignalAnalogModel->getCarrierFrequency());
    const m waveLength = propagationSpeed / carrierFrequency;
    const m distance { transmission->getStartPosition().distance(arrival->getStartPosition()) };
	const double freqAng = 2*M_PI*carrierFrequency.get();
	const double cpropagationSpeed = 299792458;

	double refDist;
    double total_loss = 0.0;
    //std::vector<double> common_pathLoss;
    std::vector<double>::iterator max, max1;
    int index, index1;
    
	if (distance.get() != 0.0) {

		// signal increase by vehicles and buildings reflection
		auto gain = compute_reflection(transmission->getStartPosition(), arrival->getStartPosition(), waveLength, 0.0);//in linear and inverted
		auto get_diffractionsRays = diffraction_rays(transmission->getStartPosition(), arrival->getStartPosition(), gain.dif_ref_dist, maxNLOSbRange);
		auto valid_rays = outer_check(transmission->getStartPosition(), arrival->getStartPosition(), get_diffractionsRays);
		auto pathLoss = compute_diffraction(valid_rays, transmission->getStartPosition(), arrival->getStartPosition(), waveLength);//in linear scale
        
        //assert(pathLoss.Efield.size() == pathLoss.dif_ref_dist.size());
        
        if(gain.Efield.size() !=0 && pathLoss.Efield.size() !=0){
        
        //common_pathLoss;
        //for(auto p : pathLoss.Efield){
        //common_pathLoss.push_back (1.0- pow(10.0, p/10.0));
        //}    
            
        max = std::max_element(pathLoss.Efield.begin(),pathLoss.Efield.end());
        index = std::distance(pathLoss.Efield.begin(), max);
        max1 = std::max_element(gain.Efield.begin(), gain.Efield.end());
        index1 = std::distance(gain.Efield.begin(), max1);
        
        //assert(common_pathLoss.size() > 0);
        //assert(gain.Efield.size() > 0);
        //assert(pathLoss.dif_ref_dist.size() > 0);
        
        if(pathLoss.Efield.at(index) > gain.Efield.at(index1)){
            refDist = pathLoss.dif_ref_dist.at(index);
        }
        else{refDist = gain.dif_ref_dist.at(index1);}
		
		for (int i = 0, j=0;i < gain.Efield.size();++i, ++j) {
			 total_loss = total_loss + (gain.Efield[i] * cos(freqAng * (refDist / (cpropagationSpeed - gain.dif_ref_dist[j])/ cpropagationSpeed))); //input in linear
		}
		for (int i = 0, j = 0;i < pathLoss.Efield.size();++i, ++j) {
			total_loss = total_loss + (pathLoss.Efield[i] * cos(freqAng * (refDist / (cpropagationSpeed - pathLoss.dif_ref_dist[j]) / cpropagationSpeed))); //input in linear
		}
		return total_loss;
        }
        
        else if(pathLoss.Efield.size() != 0 && gain.Efield.size() == 0){
        //for(auto p : pathLoss.Efield){
          //  common_pathLoss.push_back (1.0- pow(10.0, p/10.0));
        //}
        //Find the distance of the strongest path loss, set as reference distance
        max = std::max_element(pathLoss.Efield.begin(), pathLoss.Efield.end());
        index = std::distance(pathLoss.Efield.begin(), max);
        refDist = pathLoss.dif_ref_dist.at(index);
        for (int i = 0, j = 0;i < pathLoss.Efield.size();++i, ++j) {
            total_loss = total_loss + (pathLoss.Efield[i] * cos(freqAng * (refDist / (cpropagationSpeed - pathLoss.dif_ref_dist[j]) / cpropagationSpeed))); //input in linear
            return total_loss;
		}
        }
        else if(gain.Efield.size() != 0 && pathLoss.Efield.size() == 0){
            max1 = std::max_element(gain.Efield.begin(), gain.Efield.end());
            index1 = std::distance(gain.Efield.begin(), max1);
            refDist = gain.dif_ref_dist.at(index1);
            for (int i = 0, j=0;i < gain.Efield.size();++i, ++j) {
			 total_loss = total_loss + (gain.Efield[i] * cos(freqAng * (refDist / (cpropagationSpeed - gain.dif_ref_dist[j]) / cpropagationSpeed))); //input in linear
             return total_loss;
		}
        }
		else{return 0.0;}
	}
	return 1.0;
	 //linear
}

// get possible reflection rays off buidings
std::vector<Position> NLOSb::Build_reflection_rays(const ObstacleList& obstacle,const Coord& pos_t, const Coord& pos_r, double maxNLOSbRange) const //the function for both vehicle and building (cars only can be reflectors if it is taller than others)
{
    
    typedef boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> Point;
    typedef boost::geometry::model::segment<Point> Segment;
    
    std::vector<Position> b_refle_ray; // possible reflection rays
    std::vector<Point> IntersectionTest1, IntersectionTest2, IntersectionPoint;
    std::vector<Position> CatchFirstElem; //Manipulate the value
    
    //const Coord p; 
    const Coord pnext;
	
    Point MirroredPoint, Corner1, Corner2, Tx, Rx;
	Segment line_Tx_MirroredPoint, line_C1_C2, IntersPoint_Tx_Segment, IntersPoint_Rx_Segment;
    Tx.set<0>(pos_t.x);
    Tx.set<1>(pos_t.y);
	Rx.set<0>(pos_r.x);
    Rx.set<1>(pos_r.y);
    
    
    double Vy,Vx; //variable only for intermediate calculations

	for(const ObstacleIndex::Obstacle* object :  obstacle) {
		const std::vector<Position>& outline = object->getOutline();

		//closing the polygono - take into account all segments of polygono
        
		CatchFirstElem.push_back (outline.front());

		for (int i=0; i<outline.size();++i)
		{
                const Coord p{ outline[i].x.value(), outline[i].y.value() };
            if(i==outline.size()-1){
                const Coord pnext{ CatchFirstElem[0].x.value(), CatchFirstElem[0].y.value() };
            }
            else{
                const Coord pnext{ outline[i+1].x.value(), outline[i+1].y.value() };
            }
			//Find points P which are mirrored images of Rx with respect to segment
			//Find points V which, when added to Rx, translates Rx to segment
			//Vx = ((x2-x1)*(y3-y1)-(y2-y1)*(x3-x1))/((x2-x1)^2+(y2-y1)^2)*(y2-y1);
			//Vy = ((x2-x1)*(y3-y1)-(y2-y1)*(x3-x1))/((x2-x1)^2+(y2-y1)^2)*-(x2-x1);

			Vx = ((pnext.x - p.x) * (pos_r.y - p.y) - (pnext.y - p.y) * (pos_r.x - p.x)) / ((squared(pnext.x - p.x) + squared(pnext.y - p.y)) * (pnext.y - p.y));
			Vy = ((pnext.x - p.x) * (pos_r.y - p.y) - (pnext.y - p.y) * (pos_r.x - p.x)) / ((squared(pnext.x - p.x) + squared(pnext.y - p.y)) * -(pnext.x - p.x));
			//Px = pos_r.x + 2*Vx; 
			//Py = pos_r.y + 2*Vy;
            MirroredPoint.set<0>(pos_r.x + 2 * Vx); //pode tirar
            MirroredPoint.set<1>(pos_r.y + 2 * Vy);

			//intersection check: Tx-MirroredPoint and polygon segment
            //point1.set<0>(1.0); DELETE

			//line_Tx_MirroredPoint(Tx, MirroredPoint); // segment 1 to testing
            bg::set<0, 0>(line_Tx_MirroredPoint, pos_r.x + 2 * Vx);
            bg::set<0, 1>(line_Tx_MirroredPoint, pos_r.y + 2 * Vy);
            bg::set<1, 0>(line_Tx_MirroredPoint, pos_t.x);
            bg::set<1, 1>(line_Tx_MirroredPoint, pos_t.y);
            
            //pode tirar
			Corner1.set<0>(p.x);
            Corner1.set<1>(p.y);
			Corner2.set<0>(pnext.x);
            Corner2.set<1>(pnext.y);
            
			//line_C1_C2(Corner1, Corner2); // segment 2 to testing
            bg::set<0, 0>(line_C1_C2, p.x);
            bg::set<0, 1>(line_C1_C2, p.y);
            bg::set<1, 0>(line_C1_C2, pnext.x);
            bg::set<1, 1>(line_C1_C2, pnext.y);
            
			boost::geometry::intersection(line_Tx_MirroredPoint, line_C1_C2, IntersectionPoint); // test itself

			if (IntersectionPoint.size() != 0) //else?
			{
                //IntersPoint_Tx_Segment(Tx, IntersectionPoint);
                bg::set<0, 0>(IntersPoint_Tx_Segment, pos_t.x);
                bg::set<0, 1>(IntersPoint_Tx_Segment, pos_t.y);
                bg::set<1, 0>(IntersPoint_Tx_Segment, IntersectionPoint[0].get<0>());
                bg::set<1, 1>(IntersPoint_Tx_Segment, IntersectionPoint[0].get<1>());
                
                //IntersPoint_Rx_Segment(Rx, IntersectionPoint);
                bg::set<0, 0>(IntersPoint_Rx_Segment, pos_r.x);
                bg::set<0, 1>(IntersPoint_Rx_Segment, pos_r.y);
                bg::set<1, 0>(IntersPoint_Rx_Segment, IntersectionPoint[0].get<0>());
                bg::set<1, 1>(IntersPoint_Rx_Segment, IntersectionPoint[0].get<1>());
            
				//DELETE out_inters.push_back (result);
				// checking if the points found are not being blocked by building itself
				for (int j = 0, s = outline.size(); j < s; ++j)
				{
                    const Coord c{ outline[j].x.value(), outline[j].y.value() }; 
                    const Coord cnext{ outline[(j+1) % s].x.value(), outline[(j+1) % s].y.value() }; //fecha?
                    
                    if( c== p && cnext == pnext ){
                    
					//Corner1(outline[j].x, outline[j].y); //check corners
					//Corner2(outline[j + 1].x, outline[j + 1].y);
					//line_C1_C2(Corner1, Corner2);
                    
                    bg::set<0, 0>(line_C1_C2, c.x);//NOTE - considera segmento, nao reta?
                    bg::set<0, 1>(line_C1_C2, c.y);
                    bg::set<1, 0>(line_C1_C2, cnext.x);
                    bg::set<1, 1>(line_C1_C2, cnext.y);
					
					boost::geometry::intersection(IntersPoint_Tx_Segment, line_C1_C2, IntersectionTest1);
					boost::geometry::intersection(IntersPoint_Rx_Segment, line_C1_C2, IntersectionTest2);
						if (IntersectionTest1.size() == 0 && IntersectionTest2.size() == 0)
						{
							b_refle_ray.emplace_back (IntersectionPoint[0].get<0>(), IntersectionPoint[0].get<1>());
						}
                    }
				}
			}
			IntersectionPoint.clear();
			IntersectionTest1.clear();
			IntersectionTest2.clear();
		}
		CatchFirstElem.clear();
	}
return b_refle_ray;
}

// get possible reflection rays off vehicles
std::vector<Position> NLOSb::Veh_reflection_rays(const VehicleList& vehicles,const Coord& pos_t, const Coord& pos_r, double maxNLOSbRange) const
{
    
    typedef boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> Point;
    typedef boost::geometry::model::segment<Point> Segment;
    
    std::vector<Position> v_refle_ray; // possible reflection rays
    std::vector<Point> IntersectionTest1, IntersectionTest2, IntersectionPoint;
    std::vector<Position> CatchFirstElem;
    
    const Coord pnext;
	
    Point MirroredPoint, Corner1, Corner2, Tx, Rx;
	Segment line_Tx_MirroredPoint, line_C1_C2, IntersPoint_Tx_Segment, IntersPoint_Rx_Segment;
    Tx.set<0>(pos_t.x);
    Tx.set<1>(pos_t.y);
	Rx.set<0>(pos_r.x);
    Rx.set<1>(pos_r.y);
    
    double Vy,Vx; //variable only for intermediate calculations

	for(const VehicleIndex::Vehicle* object :  vehicles) {
		const std::vector<Position>& outline = object->getOutline();
         const double height = object->getHeight();

		if(height > pos_t.z || height > pos_r.z ){ //reflecting vehicle must be taller than the lower commun pair vehicle heigh + its antenna height
		
		//closing the polygono - take into account all segments of polygono
        CatchFirstElem.push_back (outline.front());

		for (int i=0; i<outline.size();++i)
		{
            
            const Coord p { outline[i].x.value(), outline[i].y.value() };
            if(i==outline.size()-1){
                const Coord pnext{ CatchFirstElem[0].x.value(), CatchFirstElem[0].y.value() };
            }
            else{
                const Coord pnext{ outline[i+1].x.value(), outline[i+1].y.value() };
            }
            
			//Find points P which are mirrored images of Rx with respect to segment
			//Find points V which, when added to Rx, translates Rx to segment
			//Vx = ((x2-x1)*(y3-y1)-(y2-y1)*(x3-x1))/((x2-x1)^2+(y2-y1)^2)*(y2-y1);
			//Vy = ((x2-x1)*(y3-y1)-(y2-y1)*(x3-x1))/((x2-x1)^2+(y2-y1)^2)*-(x2-x1);

			Vy = ((pnext.x - p.x) * (pos_r.y - p.y) - (pnext.y - p.y) * (pos_r.x - p.x)) / (squared(pnext.x - p.x) + squared(pnext.y - p.y) * (pnext.y - p.y));
			Vy = ((pnext.x - p.x) * (pos_r.y - p.y) - (pnext.y - p.y) * (pos_r.x - p.x)) / (squared(pnext.x - p.x) + squared(pnext.y - p.y) * -(pnext.x - p.x));
            
			//Px = pos_r.x + 2*Vx; 
			//Py = pos_r.y + 2*Vy;
            MirroredPoint.set<0>(pos_r.x + 2 * Vx);
            MirroredPoint.set<1>(pos_r.y + 2 * Vy);

			//intersection check: Tx-MirroredPoint and polygon segment
            //line_Tx_MirroredPoint(Tx, MirroredPoint); // segment 1 to testing
            bg::set<0, 0>(line_Tx_MirroredPoint, pos_r.x + 2 * Vx);
            bg::set<0, 1>(line_Tx_MirroredPoint, pos_r.y + 2 * Vy);
            bg::set<1, 0>(line_Tx_MirroredPoint, pos_t.x);
            bg::set<1, 1>(line_Tx_MirroredPoint, pos_t.y);
            
            Corner1.set<0>(p.x);
            Corner1.set<1>(p.y);
			Corner2.set<0>(pnext.x);
            Corner2.set<1>(pnext.y);
            
			//line_C1_C2(Corner1, Corner2); // segment 2 to testing
            bg::set<0, 0>(line_C1_C2, p.x);
            bg::set<0, 1>(line_C1_C2, p.y);
            bg::set<1, 0>(line_C1_C2, pnext.x);
            bg::set<1, 1>(line_C1_C2, pnext.y);

			boost::geometry::intersection(line_Tx_MirroredPoint, line_C1_C2, IntersectionPoint); // test itself

			if (IntersectionPoint.size() != 0) 
			{
                    //IntersPoint_Tx_Segment(Tx, IntersectionPoint);
                    bg::set<0, 0>(IntersPoint_Tx_Segment, pos_t.x);
                    bg::set<0, 1>(IntersPoint_Tx_Segment, pos_t.y);
                    bg::set<1, 0>(IntersPoint_Tx_Segment, IntersectionPoint[0].get<0>());
                    bg::set<1, 1>(IntersPoint_Tx_Segment, IntersectionPoint[0].get<1>());
                    
					//IntersPoint_Rx_Segment(Rx, IntersectionPoint);
                    bg::set<0, 0>(IntersPoint_Rx_Segment, pos_r.x);
                    bg::set<0, 1>(IntersPoint_Rx_Segment, pos_r.y);
                    bg::set<1, 0>(IntersPoint_Rx_Segment, IntersectionPoint[0].get<0>());
                    bg::set<1, 1>(IntersPoint_Rx_Segment, IntersectionPoint[0].get<1>());
                
				//DELETE out_inters.push_back (result);
				// checking if the points found are not being blocked by vehicle itself
				for (int j = 0,s=outline.size();j<s;++j)
				{
                    const Coord c { outline[j].x.value(), outline[j].y.value() };
                    const Coord cnext { outline[(j+1) %s].x.value(), outline[(j+1) %s].y.value() };
                    
                    if(c==p && cnext==pnext){
                    
                    //line_C1_C2(Corner1, Corner2);
                    bg::set<0, 0>(line_C1_C2, c.x);
                    bg::set<0, 1>(line_C1_C2, c.y);
                    bg::set<1, 0>(line_C1_C2, cnext.x);
                    bg::set<1, 1>(line_C1_C2, cnext.y);
                    
					
					boost::geometry::intersection(IntersPoint_Tx_Segment, line_C1_C2, IntersectionTest1);
					boost::geometry::intersection(IntersPoint_Rx_Segment, line_C1_C2, IntersectionTest2);
                    
						if (IntersectionTest1.size() == 0 && IntersectionTest2.size() == 0)
						{
							v_refle_ray.emplace_back (IntersectionPoint[0].get<0>(), IntersectionPoint[0].get<1>());
						}
				}
                }
			}
			IntersectionPoint.clear();
            IntersectionTest1.clear();
			IntersectionTest2.clear();
		}
		 CatchFirstElem.clear();
    }
	}
return v_refle_ray;
}

//checking if the possible reflection points are not obstructed by other object (buildings, vehicle)
std::vector<Position> NLOSb::outer_check(const Coord& pos_t, const Coord& pos_r, std::vector <Position> RaysToTest) const
{
     std::vector<Position> final_rays; // reflection points
     std::vector<const VehicleIndex::Vehicle*> Veh_Obs;
     std::vector<const ObstacleIndex::Obstacle*> Build_Obs;
     //const VehicleList& Veh_Obs;
     //const ObstacleList& Build_Obs;
     
    //testing buildings and vehicles
    for(auto ray : RaysToTest)
    {
        Build_Obs = mObstacleIndex->getObstructingObstacles(Position {pos_t.x, pos_t.y}, ray);
        if(Build_Obs.size()==0);
        {
           Build_Obs = mObstacleIndex->getObstructingObstacles(Position {pos_r.x, pos_r.y},ray);
            if(Build_Obs.size()==0);
            {
                Veh_Obs = mVehicleIndex->getObstructingVehicles(Position {pos_t.x, pos_t.x},ray);
                    if(Veh_Obs.size()==0);
                    {
                        Veh_Obs = mVehicleIndex->getObstructingVehicles(Position {pos_r.x, pos_r.y},ray); 
                            if(Veh_Obs.size()==0);
                            {
                                final_rays.emplace_back (ray.x, ray.y);
                            }

                    }
            }
    }
        Build_Obs.clear(); //keep the vector always empty so that next time does not have any remaining from former iteration
        Veh_Obs.clear(); //keep the vector always empty
    }
    return final_rays;
}

NLOSb::returns NLOSb::compute_reflection(const Coord& pos_t, const Coord& pos_r, const m lambda, int polarization) const //linear inverted pathloss as feedback
{
        
    std::vector<double> theta; // angle between reflection rays
	std::vector<double> reflCoeff; // Reflection coefficient
	std::vector<double> ref_dist; // real reflection ray distance - considering the vehicles height
	std::vector<Position> real_rays;
    std::vector<double> dif_ref_dist;
    std::vector<Position> possible_rays;

	typedef boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> Point;
	Point Tx, Rx;
    Tx.set<0>(pos_t.x);
    Tx.set<1>(pos_t.y);
	Rx.set<0>(pos_r.x);
    Rx.set<1>(pos_r.y);
    
    double reflRayPermittivity;
    
    // considering reflection and both cases: building first condition and vehicle which is second condition. 
    for(int i=1; i<3;++i){ 
        if(i==1){
            auto Build_Ell = mObstacleIndex->obstaclesEllipse(Position{ pos_t.x, pos_t.y }, Position{ pos_r.x, pos_r.y },maxNLOSbRange);
            auto possible_rays = Build_reflection_rays(Build_Ell, pos_t, pos_r, maxNLOSbRange);
            real_rays = outer_check(pos_t,pos_r, possible_rays);
            reflRayPermittivity = buildingReflRelPerm;
        }
        
         if(i==2){
            auto Veh_Ell = mVehicleIndex->vehiclesEllipse(Position{ pos_t.x, pos_t.y }, Position{ pos_r.x, pos_r.y },maxNLOSbRange);
            auto possible_rays = Veh_reflection_rays(Veh_Ell, pos_t, pos_r, maxNLOSbRange); 
            real_rays = outer_check(pos_t,pos_r, possible_rays);
            reflRayPermittivity = vehReflRelPerm;
        }
        
        if(real_rays.size()!=0){ 
       
        // calculating the incidence angle
        for (auto v: real_rays)
        {
            //x1=v.x-pos_t.x 
            //y1=v.y-pos_t.y
            //x2=v.x-pos_r.x
            //y2=v.y-pos_r.y
            //theta.push_back = abs(atan2(x1*y2-y1*x2,x1*x2+y1*y2)); equation applied for finding the angle
            
            Coord p {v.x.value(), v.y.value()};
            
            theta.push_back ((M_PI-(abs(atan2((p.x-pos_t.x)*(p.y-pos_r.y)-(p.y-pos_t.y)*(p.x-pos_r.x),(p.x-pos_t.x)*(p.x-pos_r.x)+(p.y-pos_t.y)*(p.y-pos_r.y)))))/2); // in radians 
        }
        
        // polarization calculation, need to know if it is vehicle or building because of relative permittivity
        for(auto a: theta)
        {
            if (polarization == 0)   
            {
                reflCoeff.push_back ((sin(a) - sqrt(reflRayPermittivity-squared(cos(a))))/(sin(a) + sqrt(reflRayPermittivity-squared(cos(a))))); // angle must be in radians
            }
            else
            {
                reflCoeff.push_back ((-reflRayPermittivity *sin(a) + sqrt(reflRayPermittivity-squared(cos(a)))) / (reflRayPermittivity *sin(a) + sqrt(reflRayPermittivity-squared(cos(a))))); // angle must be in radians
            }
    
        }
        
        // reflected ray distance considering the vehicle height
        for(auto d: real_rays)
        {	
            //check equation, specially subtration between vehicle heights
            ref_dist.push_back (sqrt(squared(boost::geometry::distance(Tx, d) + boost::geometry::distance(Rx, d)) + squared((pos_t.z- pos_r.z)))); //point and position type
            dif_ref_dist.push_back (boost::geometry::distance(Tx, d) + boost::geometry::distance(Rx, d)); // just to use in diffraction method NOTE 
        }
        real_rays.clear();
        possible_rays.clear();
    }
    }
        //Get power in Watts
        //double Pt = (pow(10,(txPower / 10)) / 1000);
        //Set reference distance in meters
        //double d0 = 1.0;
        //Convert antenna gain at Rx from dBi to linear
        //double Gt = pow(10,(Gt / 10)); //NOTE
        //Calculate reference power flux density at distance d0
        //double Pd0 = txPower*Gt / (4*M_PI*squared(d0));
        //Calculate reference E - field
        //double E0 = sqrt(Pd0 * 120 * M_PI);
        
        std::vector<double> ReceivedPowerW;
        //Calculate Power in Watts
        for(int i=0; i<reflCoeff.size();++i){
            
            ReceivedPowerW.push_back ((30.0*squared(reflCoeff[i])*squared(lambda.get()))/(480.0*squared(M_PI)*squared(ref_dist[i])));
        }
        
        return returns { std::move(ReceivedPowerW), std::move(dif_ref_dist) };
}
// Diffraction

//calculate the shortest reflection ray distance and finding diffraction rays
std::vector<Position> NLOSb::diffraction_rays(const Coord& pos_t, const Coord& pos_r, std::vector<double> dif_ref_dist, double maxNLOSbRange) const //checar o retorno da classe
{
    std::vector<Position> valid_corner;
    const Coord TxRxMiddlePoint { (pos_t.x+pos_r.x)/2.0, (pos_t.y+pos_r.y/2.0) };
    const Coord p_next;
    double MiddlePointOnTxP, PnextOnTxP, MiddlePointOnRxP, PnextOnRxP;
    //std::vector<double> iterator* 
    auto short_ref_ray_iter = std::min_element(dif_ref_dist.begin(), dif_ref_dist.end()); 
    double short_ref_ray = short_ref_ray_iter == dif_ref_dist.end() ? maxNLOSbRange : *short_ref_ray_iter;
    short_ref_ray = std::min(maxNLOSbRange, short_ref_ray);
        
    // the shortest reflection ray distance is used to find the obstructing buildings.
    auto obstructing_buildings = mObstacleIndex->obstaclesEllipse (Position { pos_t.x, pos_t.y }, Position { pos_r.x, pos_r.y }, short_ref_ray); 

    
    // Finding the feasible corners - There are two tests: 'Between' and intercepts the transmitter and receiver
    
    for(const ObstacleIndex::Obstacle* building : obstructing_buildings){
        const std::vector<Position>& corners = building->getOutline(); 
    
        std::vector<Position>::const_iterator it;
        for (it = corners.begin(); it != corners.end(); ++it) {
            const Coord p { it->x.value(), it->y.value() };
            
            //testing two conditions to ensuring that the points are between transmitter and receiver - method coM_PIed of the gemv2 
            double firstIf = (pos_r.x-pos_t.x) * (p.x-pos_t.x) + (pos_r.y-pos_t.y) * (p.y-pos_t.y);
            double secondIf = (pos_r.x-pos_t.x) * (p.x-pos_r.x) + (pos_r.y-pos_t.y) * (p.y-pos_r.y);
            if (firstIf >= 0.0 && secondIf <= 0.0 ){
            
            // Use triplet test to verify where it is the point (C) in relation to main segment (A,B), left ou right according to variable signal
            // z= Ax(By-Cy) + Bx(Cy-Ay) + Cx(Ay-By)
            
                if(it == corners.end()-1){
                    const Coord p_next { corners[0].x.value() , corners[0].y.value() }; //check sintaxe ?????
                }
                else{
                    const Coord p_next { (it+1)->x.value(), (it+1)->y.value() };
                }
                
                //(Tx,p,TxRxMiddlePoint)
                MiddlePointOnTxP = pos_t.x*(p.y-TxRxMiddlePoint.y) + p.x*(TxRxMiddlePoint.y-pos_t.y) + TxRxMiddlePoint.x*(pos_t.y-p.y);
                //(Tx,p,p_next)
                PnextOnTxP = pos_t.x*(p.y-p_next.y) + p.x*(p_next.y-pos_t.y) + p_next.x*(pos_t.y-p.y);
                
                if(MiddlePointOnTxP * PnextOnTxP > 0){
                
                    //(Rx,p,TxRxMiddlePoint)
                    MiddlePointOnRxP = pos_r.x*(p.y-TxRxMiddlePoint.y) + p.x*(TxRxMiddlePoint.y-pos_r.y) + TxRxMiddlePoint.x*(pos_r.y-p.y);
                    //(Rx,p,p_next)
                    PnextOnRxP = pos_r.x*(p.y-p_next.y) + p.x*(p_next.y-pos_r.y) + p_next.x*(pos_r.y-p.y);
            
                     if(MiddlePointOnRxP * PnextOnRxP > 0){
                         valid_corner.emplace_back(p.x, p.y);
                    }
                }
        }
    }
        
    }
    return valid_corner;
}
// Goes to member function of outer_check and return rays.


NLOSb::returns NLOSb::compute_diffraction (std::vector<Position> rays, const Coord& pos_t, const Coord& pos_r, m lambda) const
{
	std::vector<double> Attenuation_Diffraction;
    std::vector<double> DiffRayDistance; 
    
    for(auto ray : rays){
        
        const Coord cray { ray.x.value(), ray.y.value() }; //ray in coord type to be able perform calculations
        
        //finding the distance between corner and TxRxsegment and projection of the corner on the TxRxsegment (Corner,Tx,Rx)
        double Vx = ((pos_r.x-pos_t.x) * (cray.y-pos_t.y) - (pos_r.y-pos_t.y) * (cray.x-pos_t.x)) / (squared(pos_r.x-pos_t.x) + squared(pos_r.y-pos_t.y)) * (pos_r.y-pos_t.y); //only variable for calculation
        double Vy = ((pos_r.x-pos_t.x) * (cray.y-pos_t.y) - (pos_r.y-pos_t.y) * (cray.x-pos_t.x)) / (squared(pos_r.x-pos_t.x) + squared(pos_r.y-pos_t.y)) * -(pos_t.x-pos_t.x); //only variable for calculation
        
        //meter Vxx (Vx);
        //meter Vyy (Vy);
        
		const Coord projection { cray.x + Vx, cray.y + Vy }; //check the sintaxe
        double CornerHeight = sqrt(squared(Vx)+ squared(Vy));
        
        //distance between corner projected and transmitter
        double DistanceProjTx = sqrt(squared(pos_t.y-projection.y) + squared(pos_t.x-projection.x));
        
        //distance between corner and transmitter
        double DistanceTxCorner = sqrt(squared(pos_t.y-cray.y) + squared(pos_t.x-cray.x));
        
        //distance between coner and receiver
        const double DistanceRxCorner = sqrt(squared(pos_r.y-cray.y) + squared(pos_r.x-cray.x)); 
        
        //real LOS distance between transmitter and receiver considers vehicle height and antenna height
         const double RealDistanceTxRx = (sqrt(squared(sqrt(squared(pos_t.y-pos_r.y)+squared(pos_t.x-pos_r.x))) + squared((pos_t.z)-(pos_r.z))));
         
         //diffraction ray distance
        DiffRayDistance.push_back (sqrt(squared(DistanceTxCorner + DistanceRxCorner) + squared(pos_t.z-pos_r.z)));
         
		 // free space loss
		 double loss = (16.0 * squared(M_PI) * squared(RealDistanceTxRx)) / (squared(lambda.get())); //path loss in linear
        
		 Attenuation_Diffraction.push_back (1.0/(loss*(computeSimpleKnifeEdgeCorner(CornerHeight, RealDistanceTxRx,DistanceProjTx,lambda)))); // sum the losses.
    }
    
	return returns { std::move(Attenuation_Diffraction), std::move(DiffRayDistance) };
}

// I call of projection the diffraction point onto TxRx line in order to calculate the distance between Tx and this point
double NLOSb::computeSimpleKnifeEdgeCorner(double CornerHeight, double RealDistanceTxRx, double DistanceProjTx, m lambda) const
{
    // following calculations are similar to equation 29 of ITU-R P.526-13:
    //  v = sqrt(2d/lambda * alpha1 * alpha2)
    //
    // with sinus approximation for small angles:
    //  alpha1 =~ sin alpha1 = h / d1
    //  alpha2 =~ sin alpha2 = h / d2
    //
    //  v = sqrt(2d / lambda * h / d1 * h / d2) = sqrt(2) * h / sqrt(lambda * d1 * d2 / d)
    //
    // with d1 = distTxObs, d2 = distRxObs, d = distTxRx,

    // distance between Rx and corner projected
    const double DistanceProjRx = RealDistanceTxRx - DistanceProjTx;
    // calculate the Fresnel ray
    const double r = sqrt(lambda.get() * DistanceProjTx * DistanceProjRx / RealDistanceTxRx);
    static const double root_two = sqrt(2.0);
    const double v = root_two * static_cast<inet::unit>(CornerHeight / r).get();

    double loss = 0.0;
    if (v > -0.78) {
        // approximation of Fresnel-Kirchoff loss given by ITU-R P.526, equation 31 (result in dB):
        // J(v) = 6.9 + 20 log(sqrt((v - 01)^2 + 1) + v - 0.1)
        loss = inet::math::dB2fraction (6.9 + 20.0 * log10(sqrt(squared(v - 0.1) + 1.0) + v - 0.1));
    }

    return loss;
}


} // namespace gemv2
} // namespace artery
