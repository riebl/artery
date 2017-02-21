#include "artery/utility/Geometry.h"
#include <boost/geometry/algorithms/distance.hpp>

double Angle::radian() const
{
    return value / boost::units::si::radian;
}

double Angle::degree() const
{
    boost::units::quantity<boost::units::degree::plane_angle> tmp { value };
    return tmp / boost::units::degree::degree;
}

Position::value_type distance(const Position& a, const Position& b)
{
    auto d = boost::geometry::distance(a, b);
    return d * boost::units::si::meter;
}

bool operator==(const Position& a, const Position& b)
{
    return !(a != b);
}

bool operator!=(const Position& a, const Position& b)
{
    return a.x != b.x || a.y != b.y;
}
