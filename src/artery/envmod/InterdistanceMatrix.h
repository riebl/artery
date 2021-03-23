/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_ENVMOD_INTERDISTANCEMATRIX_H_TDSA1OVD
#define ARTERY_ENVMOD_INTERDISTANCEMATRIX_H_TDSA1OVD

#include "artery/envmod/PreselectionMethod.h"
#include "artery/envmod/Geometry.h"
#include <list>
#include <unordered_map>

namespace artery
{

class EnvironmentModelObject;

/**
 * The distance to the middle of the object and the angle is checked.
 * Not as exact as the other methods because only the center of the object
 * will be checked, disregarding the vehicle frame.
 */
class InterdistanceMatrix : public PreselectionMethod
{
public:
    using PreselectionMethod::PreselectionMethod;

    void update() override;
    std::vector<std::string> select(const EnvironmentModelObject& ego, const SensorConfigFov&) override;

private:
    struct ItemSelector
    {
        double left = 0.0;
        double right = 0.0;
        double range = 0.0;
    };

    struct MatrixItem
    {
        MatrixItem(double distanceToY_, double angleToY_);

        double distanceToY = 0.0;
        double angleToY = 0.0;
        double radius = 0.0;
        double relX = 0.0;
        double relY = 0.0;
        std::string name;
    };

    ItemSelector buildItemSelector(const SensorConfigFov&) const;
    bool checkItemSelector(const ItemSelector&, const MatrixItem&) const;

    /**
     * Calculates distance between ego object and another object.
     * @param egoPoint ego position
     * @param otherPoint other object's position
     * @return Euclidian distance in meter
     */
    double calcDistanceEgoToX(const geometry::Point& egoPoint, const geometry::Point& otherPoint) const;

    /**
     * Calculates angle between ego object and another object.
     * The angle is calculated under consideration of the driving direction of the ego vehicle.
     * @param objEgo Ego object
     * @param objX other object
     * @return angle in degree
     */
    double calcAngleEgoToX(const EnvironmentModelObject& objEgo, const EnvironmentModelObject& objX) const;

    std::unordered_map<std::string, std::list<MatrixItem>> mMatrix;
};

} // namespace artery

#endif /* ARTERY_ENVMOD_INTERDISTANCEMATRIX_H_TDSA1OVD */
