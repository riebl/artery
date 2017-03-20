#include "artery/storyboard/TtcCondition.h"
#include "artery/storyboard/Vehicle.h"
#include <boost/geometry.hpp>
#include <boost/range/algorithm_ext/for_each.hpp>
#include <boost/units/cmath.hpp>
#include <boost/units/systems/si/length.hpp>
#include <boost/units/systems/si/time.hpp>


TtcCondition::TtcCondition(double ttc) :
    m_ttc(ttc), m_ttcDistanceThreshold(100)
{
}

TtcCondition::TtcCondition(double ttc, double ttcDistanceThreshold) :
    m_ttc(ttc), m_ttcDistanceThreshold(ttcDistanceThreshold)
{
}

TtcCondition::CarShape TtcCondition::getShape(Position pos, double angle, double length, double width) const
{
    namespace units = boost::units::si;

    const double halfLength = length * 0.5;
    const double halfWidth = width * 0.5;
    const double heading = M_PI / 2 - angle;
    const double sinH = sin(heading);
    const double cosH = cos(heading);

    Position fm(( cosH * halfLength * units::meters + pos.x), (-sinH * halfLength * units::meters + pos.y));
    Position fl((-sinH * halfWidth  * units::meters + fm.x), (-cosH * halfWidth * units::meters + fm.y));
    Position fr(( sinH * halfWidth  * units::meters + fm.x), ( cosH * halfWidth * units::meters + fm.y));

    Position rm((-cosH * halfLength * units::meters + pos.x), (sinH * halfLength *units::meters + pos.y));
    Position rr(( sinH * halfWidth  * units::meters + rm.x), ( cosH * halfWidth * units::meters + rm.y));
    Position rl((-sinH * halfWidth  * units::meters + rm.x), (-cosH * halfWidth * units::meters + rm.y));

    CarShape shape;
    shape.setFrontRight(fr);
    shape.setRearRight(rr);
    shape.setRearLeft(rl);
    shape.setFrontLeft(fl);

    return shape;
}

TtcCondition::Route TtcCondition::calculateRoute(const Vehicle& car, int steps, double dt) const
{
    std::vector<CarShape> shapes;
    auto& controller = car.getController();
    auto& vdp = car.get<VehicleDataProvider>();
    const double width = controller.getWidth() / boost::units::si::meters;
    const double length = controller.getLength() / boost::units::si::meters;
    auto carHeading = car.get<VehicleDataProvider>().heading();
    const auto carPosition = controller.getPosition();

    // Calculate carCenter because controller.getPosition() returns the front middle position of the car
    Position carCenter;
    carCenter.x = -cos(M_PI / 2 - carHeading.value()) * length / 2 * boost::units::si::meters + carPosition.x;
    carCenter.y = sin(M_PI / 2 - carHeading.value()) * length / 2 * boost::units::si::meters + carPosition.y;

    // Calculate car positions following the expected movement of the car while not driving straight
    if (boost::units::abs(vdp.curvature()) > 0 * vanetza::units::reciprocal_metre) {
        auto radius = 1.0 / vdp.curvature();
        Position curveCenter;
        curveCenter.x = -radius * boost::units::cos(carHeading) + carCenter.x;
        curveCenter.y = -radius * boost::units::sin(carHeading) + carCenter.y;
        const auto angleG = vdp.yaw_rate() * dt * boost::units::si::seconds;

        for (int i = 1; i <= steps; i++) {
            Position nextPosition;
            auto angleD = (carHeading - angleG);
            nextPosition.x = radius * boost::units::cos(angleD) + curveCenter.x;
            nextPosition.y = radius * boost::units::sin(angleD) + curveCenter.y;

            carHeading = carHeading - angleG;
            shapes.push_back(getShape(nextPosition, carHeading / boost::units::si::radians , length, width));
        }
    // Calculations for strait driving car
    } else {
        const auto heading = M_PI / 2 * boost::units::si::radians - vdp.heading();
        const double sinH = boost::units::sin(heading);
        const double cosH = boost::units::cos(heading);

        for (int i = 1; i <= steps; i++) {
            auto distance = vdp.speed() * dt * boost::units::si::seconds;
            Position m((cosH * i * distance + carCenter.x), (-sinH * i * distance+ carCenter.y));
            shapes.push_back(getShape(m, vdp.heading() / boost::units::si::radians, length, width));
        }
    }

    return shapes;
}

double TtcCondition::calculateTimeDelta(const Vehicle& car1, const Vehicle& car2) const
{
    auto& vdp1 = car1.get<VehicleDataProvider>();
    auto& vdp2 = car2.get<VehicleDataProvider>();
    const double dT1 = (car1.getController().getLength() / boost::units::si::meters) / (vdp1.speed() / boost::units::si::meter_per_second);
    const double dT2 = (car2.getController().getLength() / boost::units::si::meters) / (vdp2.speed() / boost::units::si::meter_per_second);

    // return time delta calculated by the faster car to avoid gaps between vehicle shapes
    return std::min(dT1, dT2);
}

bool TtcCondition::calculateIntersect(const CarShape& shape1, const CarShape& shape2) const
{
    boost::geometry::model::polygon<Position, true, false> polygon1;
    boost::geometry::assign_points(polygon1, shape1);

    boost::geometry::model::polygon<Position, true, false> polygon2;
    boost::geometry::assign_points(polygon2, shape2);

    return boost::geometry::intersects(polygon1, polygon2);
}

bool TtcCondition::intersect(const Vehicle& car1, const Vehicle& car2)
{
    bool no_intersection = true;
    if (boost::geometry::distance(car1.getController().getPosition(), car2.getController().getPosition()) <= m_ttcDistanceThreshold) {
        const double dt = calculateTimeDelta(car1, car2);
        int steps = ceil(m_ttc / dt);

        mEgoRoute = calculateRoute(car1, steps, dt);
        mOthersRoute.push_back(calculateRoute(car2, steps, dt));

        no_intersection = boost::range::equal(mEgoRoute, mOthersRoute.back(),
                [this](CarShape s1, CarShape s2) {
                    return !calculateIntersect(s1, s2);
                }
        );
    }
    return !no_intersection;
}

ConditionResult TtcCondition::testCondition(const Vehicle& car)
{
    mEgoRoute.clear();
    mOthersRoute.clear();
    std::set<const Vehicle*> affected;
    for (auto& testCar : car.getVehicles()) {
        if (testCar.first != car.getId()) {
            if (intersect(car, testCar.second)) {
                affected.insert(&testCar.second);
            }
        }
    }
    return affected;
}

void TtcCondition::drawCondition(omnetpp::cCanvas* canvas)
{
    for(auto& fig : mFigures) {
        canvas->removeFigure(fig);
        delete fig;
    }
    mFigures.clear();

    drawPath(mEgoRoute, canvas);
    for(auto& p : mOthersRoute) {
        drawPath(p, canvas);
    }
}

void TtcCondition::drawPath(const Route& route, omnetpp::cCanvas* canvas)
{
    for(const auto &shape : route) {

        omnetpp::cPolygonFigure* fig = new omnetpp::cPolygonFigure();
        for(const auto& point : shape) {
            fig->addPoint(omnetpp::cFigure::Point(point.x.value(), point.y.value()));
        }

        canvas->addFigure(fig);
        mFigures.push_back(fig);
    }
}
