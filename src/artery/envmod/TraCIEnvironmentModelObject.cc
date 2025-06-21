/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/StationType.h"
#include "artery/envmod/TraCIEnvironmentModelObject.h"
#include "artery/traci/PersonController.h"
#include "artery/traci/VehicleController.h"
#include <boost/geometry/strategies/transform/matrix_transformers.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/units/cmath.hpp>
#include <boost/units/systems/angle/degrees.hpp>
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
transformObject(double length, double width, const Position& pos, Angle alpha)
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
transformObject(double length, double width, const Position& pos, Angle alpha)
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

TraCIEnvironmentModelObject::TraCIEnvironmentModelObject(const traci::Controller* controller, uint32_t id) :
    VehicleDataProvider(id),
    mController(controller)
{
    mLength = controller->getLength();
    mWidth = controller->getWidth();
    const auto halfWidth = mWidth * 0.5;
    const auto halfLength = mLength * 0.5;
    mRadius = sqrt(halfWidth * halfWidth + halfLength * halfLength);

    auto vehicle = dynamic_cast<const traci::VehicleController*>(controller);
    if  (vehicle) {
        setStationType(deriveStationTypeFromVehicleClass(vehicle->getVehicleClass()));
    }
    else {
        setStationType(StationType::Pedestrian);
    }
    update();
}

const VehicleDataProvider& TraCIEnvironmentModelObject::getVehicleData() const
{
   return *this;
}

void TraCIEnvironmentModelObject::update()
{
    // Update the internal vdp
    VehicleDataProvider::update(getKinematics(*mController));

    // Recalculate all time and position dependent attributes
    using namespace boost::math::double_constants;
    Angle heading = -1.0 * (getVehicleData().heading() - 0.5 * pi * boost::units::si::radian);
    auto affine = transformObject(mLength.value(), mWidth.value(), getVehicleData().position(), heading);

    boost::geometry::transform(squareCentrePoint, mCentrePoint, affine);
    mOutline.clear();
    boost::geometry::transform(squareOutline, mOutline, affine);
    mAttachmentPoints.clear();
    boost::geometry::transform(squareAttachmentPoints, mAttachmentPoints, affine);
}

EnvironmentModelObject::Heading TraCIEnvironmentModelObject::getHeading() const
{
    using boost::units::si::radians;
    static const auto pi = boost::math::constants::pi<double>();

    // heading from vehicle data is headed north (clockwise),
    vanetza::units::Angle traci_heading = getVehicleData().heading();
    // OMNeT++ angles are headed east (counter-clockwise)
    Angle opp_heading = 0.5 * pi * radians - traci_heading;
    return opp_heading;
}

std::string TraCIEnvironmentModelObject::getExternalId() const
{
    return mController->getId();
}

bool TraCIEnvironmentModelObject::isVisible() {
    if (getStationType() == StationType::Pedestrian) {
        return !dynamic_cast<const traci::PersonController *>(mController)->isDriving();
    }
    else {
        return true;
    }
}

} // namespace artery
