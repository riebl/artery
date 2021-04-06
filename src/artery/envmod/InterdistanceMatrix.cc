/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/envmod/InterdistanceMatrix.h"
#include "artery/envmod/Geometry.h"
#include "artery/envmod/sensor/SensorConfiguration.h"
#include <boost/units/systems/angle/degrees.hpp>
#include <cmath>

namespace geom = boost::geometry;
using boost::units::degree::degrees;

namespace artery
{

namespace {

/**
 * Calculates the angle between two points using an centre point
 * @param p1 Point 1
 * @param p2 Point 2
 * @param centre Centre point
 * @return Angle between the two points
 */
Angle createAngle(geometry::Point p1, geometry::Point p2, geometry::Point centre)
{
    geom::subtract_point(p1, centre);
    geom::subtract_point(p2, centre);

    double angle1 = atan2(p1.get<0>(), p1.get<1>());
    double angle2 = atan2(p2.get<0>(), p2.get<1>());

    using namespace boost::math::double_constants;
    double radAngle { angle2 - angle1 }; // range [-2pi; 2pi]
    if (radAngle > pi) {
        radAngle -= 2.0 * pi;
    } else if (radAngle < -pi) {
        radAngle += 2.0 * pi;
    }
    return radAngle * boost::units::si::radian;
}

}

void InterdistanceMatrix::update()
{
    mMatrix.clear();

    for (const auto& objEgo : mObjects) {
        std::list<MatrixItem> xs;
        for (const auto& objX : mObjects) {
            if (objEgo->getExternalId() != objX->getExternalId()) {
                const double distance = geom::distance(objEgo->getCentrePoint(), objX->getCentrePoint());
                double angle = calcAngleEgoToX(*objEgo, *objX);
                assert(angle <= 180.0 && angle >= -180.0);

                xs.emplace_back(distance, angle);
                xs.back().name = objX->getExternalId();
                xs.back().radius = objX->getRadius().value();
            }
        }

        mMatrix.emplace(objEgo->getExternalId(), std::move(xs));
    }
}

std::vector<std::string> InterdistanceMatrix::select(const EnvironmentModelObject& ego, const SensorConfigFov& config)
{
    assert(ego.getExternalId() == config.egoID);
    ItemSelector selector = buildItemSelector(config);

    std::vector<std::string> objectsInVecinityID;

    auto ego_found = mMatrix.find(config.egoID);
    if (ego_found != mMatrix.end()) {
        for (const auto& item : ego_found->second) {
            if (checkItemSelector(selector, item)) {
                objectsInVecinityID.push_back(item.name);
            }
        }
    } else {
        throw std::runtime_error("No entry found for " + config.egoID);
    }

    return objectsInVecinityID;
}

InterdistanceMatrix::ItemSelector InterdistanceMatrix::buildItemSelector(const SensorConfigFov& config) const
{
    if (config.egoID.empty())
        throw std::runtime_error("Vehicle ID empty");

    if (config.fieldOfView.range.value() == 0.0)
        throw std::runtime_error("Sensor range is 0");

    if (config.fieldOfView.angle <= 0.0 * degrees)
        throw std::runtime_error("Sensor opening angle is equal or less than 0 degree");

    if (config.fieldOfView.angle > 360.0 * degrees)
        throw std::runtime_error("Sensor opening angle greater than 360 degree");

    ItemSelector selector;
    selector.range = config.fieldOfView.range / boost::units::si::meters;

    const double sensorDirection = relativeAngle(config.sensorPosition).degree();
    const double halfOpeningAngleDegree = 0.5 * config.fieldOfView.angle / degrees;
    selector.left = sensorDirection + halfOpeningAngleDegree;
    selector.right = sensorDirection - halfOpeningAngleDegree;

    if (selector.left > 180.0) {
        selector.left -= 360.0;
    } else if (selector.left <= -180.0) {
        selector.left += 360.0;
    }

    if (selector.right > 180.0) {
        selector.right -= 360.0;
    } else if (selector.right <= -180.0) {
        selector.right += 360.0;
    }

    return selector;
}

bool InterdistanceMatrix::checkItemSelector(const ItemSelector& selector, const MatrixItem& item) const
{
    // add radius because the center of the vehicle could be out of sensor range but parts of vehicle body still be within
    if (item.distanceToY <= selector.range + item.radius) {
        if (selector.left == selector.right) {
            // full 360 degree sweep
            return true;
        } else if (selector.left > selector.right) {
            return item.angleToY < selector.left && item.angleToY > selector.right;
        } else {
            return item.angleToY < selector.left || item.angleToY > selector.right;
        }
    }

    return false;
}

double InterdistanceMatrix::calcDistanceEgoToX(const geometry::Point& egoPoint, const geometry::Point& otherPoint) const
{
    return geom::distance(egoPoint, otherPoint);
}

double InterdistanceMatrix::calcAngleEgoToX(const EnvironmentModelObject& objEgo, const EnvironmentModelObject& objX) const
{
    geometry::Point reference(0.0, -1.0);

    // Rotate reference point in driving direction
    const auto& egoData = objEgo.getVehicleData();
    geom::strategy::transform::rotate_transformer<geom::radian, double, 2, 2> rotate(-1.0 * egoData.heading() / boost::units::si::radian);
    geom::transform(reference, reference, rotate);

    // Translate reference point to ego vehicle
    geom::strategy::transform::translate_transformer<double, 2, 2> translate(egoData.position().x.value(), egoData.position().y.value());
    geom::transform(reference, reference, translate);

    // Determine angle between reference and objX (ego is vertex point)
    geometry::Point objEgoPos, objXPos;
    geom::convert(egoData.position(), objEgoPos);
    geom::convert(objX.getVehicleData().position(), objXPos);
    return createAngle(reference, objXPos, objEgoPos).degree();
}

InterdistanceMatrix::MatrixItem::MatrixItem(double distanceToY_, double angleToY_) :
    distanceToY(distanceToY_), angleToY(angleToY_)
{
    relX = cos(angleToY) * distanceToY;
    relY = sin(angleToY) * distanceToY;
}

} // namespace artery
