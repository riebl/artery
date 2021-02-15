/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/traci/VehicleController.h"
#include "artery/application/StationType.h"
#include "artery/envmod/EnvironmentModelObject.h"
#include <boost/geometry/strategies/transform/matrix_transformers.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/units/cmath.hpp>
#include <omnetpp/cexception.h>

namespace artery
{

namespace {

// corner points of 1x1 square (heading east, front bumper at origin)
const std::vector<Position> squareOutline = {
    Position(0.0, 0.5), // front left
    Position(0.0, -0.5), // front right
    Position(-1.0, -0.5), // back right
    Position(-1.0, 0.5) // back left
};

// attachment points for sensor
const std::vector<Position> squareAttachmentPoints = {
    Position(0.0, 0.0), // front middle
    Position(-0.5, -0.5), // right middle
    Position(-1.0, 0.0), // back middle
    Position(-0.5, 0.5) // left middle
};

const Position squareCentrePoint(-0.5, 0.0);

#if BOOST_VERSION < 106400
boost::geometry::strategy::transform::ublas_transformer<double, 2, 2>
transformVehicle(double length, double width, const Position& pos, Angle alpha)
{
    using namespace boost::geometry::strategy::transform;
    using result_type = ublas_transformer<double, 2, 2>;
    using matrix_type = boost::numeric::ublas::matrix<double>;

    // scale square to vehicle dimensions
    scale_transformer<double, 2, 2> scaling(length, width);
    // rotate into driving direction
    rotate_transformer<boost::geometry::radian, double, 2, 2> rotation(alpha.radian());
    // move to given front bumper position
    translate_transformer<double, 2, 2> translation(pos.x.value(), pos.y.value());

    const matrix_type rotateAfterScale = prod(rotation.matrix(), scaling.matrix());
    const matrix_type translateAfterRotateAfterScale = prod(translation.matrix(), rotateAfterScale);
    return result_type { translateAfterRotateAfterScale };
}
#else
boost::geometry::strategy::transform::matrix_transformer<double, 2, 2>
transformVehicle(double length, double width, const Position& pos, Angle alpha)
{
    using namespace boost::geometry::strategy::transform;

    // scale square to vehicle dimensions
    scale_transformer<double, 2, 2> scaling(length, width);
    // rotate into driving direction
    rotate_transformer<boost::geometry::radian, double, 2, 2> rotation(alpha.radian());
    // move to given front bumper position
    translate_transformer<double, 2, 2> translation(pos.x.value(), pos.y.value());

    return matrix_transformer<double, 2, 2> { translation.matrix() * rotation.matrix() * scaling.matrix() };
}
#endif

}

EnvironmentModelObject::EnvironmentModelObject(const traci::VehicleController* vehicle, uint32_t id) :
    VehicleDataProvider(id),
    mVehicleController(vehicle),
    mLength(vehicle->getVehicleType().getLength()),
    mWidth(vehicle->getVehicleType().getWidth())
{
    const auto halfWidth = mWidth * 0.5;
    const auto halfLength = mLength * 0.5;
    mRadius = sqrt(halfWidth * halfWidth + halfLength * halfLength);

    setStationType(deriveStationTypeFromVehicleClass(mVehicleController->getVehicleClass()));
    update();
}

void EnvironmentModelObject::update()
{
    // Update the internal vdp
    VehicleDataProvider::update(getKinematics(*mVehicleController));

    // Recalculate all time and position dependent attributes
    using namespace boost::math::double_constants;
    Angle heading = -1.0 * (getVehicleData().heading() - 0.5 * pi * boost::units::si::radian);
    auto affine = transformVehicle(mLength.value(), mWidth.value(), getVehicleData().position(), heading);

    boost::geometry::transform(squareCentrePoint, mCentrePoint, affine);
    mOutline.clear();
    boost::geometry::transform(squareOutline, mOutline, affine);
    mAttachmentPoints.clear();
    boost::geometry::transform(squareAttachmentPoints, mAttachmentPoints, affine);
}

std::string EnvironmentModelObject::getExternalId() const
{
    return mVehicleController->getVehicleId();
}

const Position& EnvironmentModelObject::getAttachmentPoint(const SensorPosition& pos) const
{
    assert(mAttachmentPoints.size() == 4);
    const Position* point = nullptr;
    switch (pos) {
        case SensorPosition::FRONT:
            point = &mAttachmentPoints[0];
            break;
        case SensorPosition::BACK:
            point = &mAttachmentPoints[2];
            break;
        case SensorPosition::RIGHT:
            point = &mAttachmentPoints[1];
            break;
        case SensorPosition::LEFT:
            point = &mAttachmentPoints[3];
            break;
        default:
            throw omnetpp::cRuntimeError("Invalid sensor attachment point requested");
            break;
    }

    return *point;
}

const VehicleDataProvider& EnvironmentModelObject::getVehicleData() const
{
   return *this;
}

} // namespace artery
