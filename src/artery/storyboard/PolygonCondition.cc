#include "artery/storyboard/PolygonCondition.h"
#include <boost/geometry.hpp>
#include <boost/geometry/algorithms/within.hpp>
#include <boost/geometry/geometries/ring.hpp>
#include <boost/geometry/geometries/register/ring.hpp>
#include <omnetpp/cexception.h>

BOOST_GEOMETRY_REGISTER_RING(std::vector<Position>)

PolygonCondition::PolygonCondition(const std::vector<Position>& vertices) :
    m_vertices(vertices)
{
    using omnetpp::cRuntimeError;

    if (m_vertices.front() == m_vertices.back()) {
        throw cRuntimeError("PolygonCondition vector must not contain the first point as end point");
    }
    if (m_vertices.size() <= 3) {
        throw cRuntimeError("PolygonCondition vector must contain at least 3 points");
    }
}

bool PolygonCondition::testCondition(const Vehicle& car)
{
    const auto& point = car.controller.getPosition();
    return boost::geometry::within(point, m_vertices);
}

int PolygonCondition::edges() const {
    return m_vertices.size() - 1;
}
