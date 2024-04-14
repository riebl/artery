#include "artery/envmod/sensor/SensorConfiguration.h"
#include "artery/envmod/EnvironmentModelObject.h"
#include "artery/envmod/Geometry.h"
#include <boost/geometry/strategies/transform/matrix_transformers.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/units/cmath.hpp>

namespace artery
{

std::vector<Position> createSensorArc(const SensorConfigFov& config, const Position& egoPos, const Angle& egoHeading)
{
    namespace gm = boost::geometry;
    using rotation = gm::strategy::transform::rotate_transformer<gm::degree, double, 2, 2>;
    using translation = gm::strategy::transform::translate_transformer<double, 2, 2>;

    const double openingAngleDeg = config.fieldOfView.angle / boost::units::degree::degrees;
    unsigned segments = std::max(config.numSegments, 1u);
    const double segmentAngle = openingAngleDeg / segments;
    const double sensorPositionDeg = relativeAngle(config.sensorPosition).degree();

    std::vector<Position> points;

    const bool full_circle = config.fieldOfView.angle == 360.0 * boost::units::degree::degree;
    if (full_circle) {
        // full circle: omit last segment point to prevent nearly-overlapping geometry points
        --segments;
    } else {
        // actual cone: add center point
        points.push_back(Position {0.0, 0.0});
    }

    Position sensorBoundary(config.fieldOfView.range / boost::units::si::meters, 0.0);
    rotation rotateSensorBoundary(egoHeading.degree() - sensorPositionDeg - 0.5 * openingAngleDeg);
    gm::transform(sensorBoundary, sensorBoundary, rotateSensorBoundary);
    points.push_back(sensorBoundary);

    rotation rotateSegment(segmentAngle);
    for (unsigned i = 0; i < segments; ++i) {
        Position segmentPoint;
        gm::transform(points.back(), segmentPoint, rotateSegment);
        points.push_back(segmentPoint);
    }

    // translate sensor cone points to car position
    translation translate(egoPos.x.value(), egoPos.y.value());
    for (auto& point : points) {
        gm::transform(point, point, translate);
    }

    return points;
}

std::vector<Position> createSensorArc(const SensorConfigFov& config, const EnvironmentModelObject& egoObj)
{
    Position sensorPos = egoObj.getAttachmentPoint(config.sensorPosition);
    return createSensorArc(config, sensorPos, egoObj.getHeading());
}

} // namespace artery
