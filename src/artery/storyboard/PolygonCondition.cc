#include "artery/storyboard/PolygonCondition.h"

#undef ev
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/register/point.hpp>

BOOST_GEOMETRY_REGISTER_POINT_2D(Coord, double, cs::cartesian, x, y)

bool PolygonCondition::testCondition(const Vehicle& car)
{
    const Coord& point = car.mobility.getCurrentPosition();
    boost::geometry::model::polygon<Coord, true, false> polygon1;
    boost::geometry::assign_points(polygon1, m_vertices);
    return boost::geometry::within(point, polygon1);
}

int PolygonCondition::edges() const {
    return m_vertices.size() - 1;
}
