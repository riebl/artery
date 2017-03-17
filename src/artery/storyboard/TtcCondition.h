#ifndef TTCCONDITION_H_
#define TTCCONDITION_H_

#include "artery/storyboard/CarSetCondition.h"
#include "artery/storyboard/Condition.h"
#include "artery/utility/Geometry.h"
#include <omnetpp/ccanvas.h>
#include <vector>

class TtcCondition : public Condition {
public:

    /**
     * Structure to hold a shape of a car
     */
    struct CarShape : std::array<Position, 4>
    {
        void setFrontRight(Position p) { (*this)[0] = p; }
        void setRearRight(Position p) { (*this)[1] = p; }
        void setRearLeft(Position p) { (*this)[2] = p; }
        void setFrontLeft(Position p) { (*this)[3] = p; }

        const Position& getFrontRight() const { return (*this)[0]; }
        const Position& getRearRight() const { return (*this)[1]; }
        const Position& getRearLeft() const { return (*this)[2]; }
        const Position& getFrontLeft() const { return (*this)[3]; }
    };

    using Route = std::vector<CarShape>;

    /**
     * Creates a TtcCondition which is testing if the defined cars may collide in a given time
     *
     * \param TimeToCollision
     */
    TtcCondition(double);

    /**
     * Creates a TtcCondition which is testing if the defined cars may collide in a given time
     *
     * \param TimeToCollision
     * \param ttcDistanceThreshold
     */
    TtcCondition(double, double);

    ConditionResult testCondition(const Vehicle& car);

private:

    /**
     * Calculates the shape of a car
     *
     * \param pos position of the car
     * \param angle heading of the car in degrees from north, cw positive
     * \param length length of the car
     * \param width width of the car
     * \return shape of the car
     */
    CarShape getShape(Position, double heading, double length, double width) const;

    /**
     * Calculates the predicted route of a car
     *
     * \param car Vehicle from which the route should be calculated
     * \param steps number of steps which should be calculated
     * \param dt time between shapes
     * \return Vector containing the shapes of the calculated steps
     */
    Route calculateRoute(const Vehicle&, int steps, double dt) const;

    /**
     * Calculates the Time between two car shapes
     *
     * \param car1 first Vehicle
     * \param car2 second Vehicle
     * \return double time between two car shapes
     */
    double calculateTimeDelta (const Vehicle& first, const Vehicle& second) const;

    /**
     * Calculates if two car shapes may intersect
     *
     * \param shape1 Vector containing frontRight, rearRight, rearLeft, frontLeft Coordinates of car1
     * \param shape2 Vector containing frontRight, rearRight, rearLeft, frontLeft Coordinates of car2
     * \return true if shapes intersect
     */
    bool calculateIntersect(const CarShape& first, const CarShape& second) const;

    /**
     * Calculates if two cars may collide on their predicted route
     *
     * \param car1 first Vehicle
     * \param car2 second Vehicle
     * \return true if the cars may collide
     */
    bool intersect(const Vehicle& first, const Vehicle& second);

    double m_ttc;
    double m_ttcDistanceThreshold;

private:
    Route mEgoRoute;
    std::vector<Route> mOthersRoute;

    std::list<omnetpp::cFigure*> mFigures;

    void drawCondition(omnetpp::cCanvas*) override;
    void drawPath(const Route& route, omnetpp::cCanvas* canvas);
};

#endif /* TTCCONDITION_H_ */
