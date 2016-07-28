#include "artery/storyboard/PolygonCondition.h"

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/register/point.hpp>

BOOST_GEOMETRY_REGISTER_POINT_2D(Coord, double, cs::cartesian, x, y)

PolygonCondition::PolygonCondition(const std::vector<Coord>& vertices) :
    m_vertices(vertices)
{
    if(m_vertices.front() == m_vertices.back()) {
        throw cRuntimeError("PolygonCondition vector must not contain the first point as end point");
    }
    if(m_vertices.size() <= 3) {
        throw cRuntimeError("PolygonCondition vector must contain at least 3 points");
    }
}

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
