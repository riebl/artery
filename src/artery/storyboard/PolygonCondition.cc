#include "artery/storyboard/PolygonCondition.h"

bool PolygonCondition::testCondition(Veins::TraCIMobility* car)
{
    const Coord& point = car->getCurrentPosition();
    bool c = false;
    // Raycasting algorithm, performing the check if the point is inside the polygon
    for (unsigned i = 0, j = m_vertices.size() - 1; i < m_vertices.size(); j = i++) {
        if (((m_vertices[i].y > point.y) != (m_vertices[j].y > point.y)) &&
            (point.y < (m_vertices[j].x - m_vertices[i].x) * (point.y - m_vertices[i].y) / (m_vertices[j].y - m_vertices[i].y) + m_vertices[i].x))
            c = !c;
    }
    return c;
}

int PolygonCondition::edges() const {
    return m_vertices.size() - 1;
}
