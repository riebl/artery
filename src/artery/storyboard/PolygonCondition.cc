#include "artery/storyboard/PolygonCondition.h"
#include "artery/utility/Geometry.h"
#include <omnetpp/cexception.h>

namespace artery
{

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

    boost::geometry::correct(m_vertices);

    mDraw = true;
}

ConditionResult PolygonCondition::testCondition(const Vehicle& car)
{
    const auto& point = car.getController().getPosition();
    return boost::geometry::within(point, m_vertices);
}

int PolygonCondition::edges() const {
    return m_vertices.size() - 1;
}

void PolygonCondition::drawCondition(omnetpp::cCanvas* canvas)
{
    if(mDraw) {
        omnetpp::cPolygonFigure* fig = new omnetpp::cPolygonFigure();
        for(const auto& point : m_vertices) {
            fig->addPoint(omnetpp::cFigure::Point(point.x.value(), point.y.value()));
        }
        canvas->addFigure(fig);
        mDraw = false;
    }
}

} // namespace artery
