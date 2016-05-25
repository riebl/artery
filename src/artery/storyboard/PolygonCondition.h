#ifndef _POLYGONCONDITION_H_
#define _POLYGONCONDITION_H_

#include <vector>
#include "artery/storyboard/Condition.h"
#include "veins/base/utils/Coord.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"

class PolygonCondition : public Condition
{
public:
    /**
     * Creates a polygon area from the given Omnet++ Coordinates
     * \note The vector must not contain the first point as end point
     */
    PolygonCondition(std::vector<Coord> vertices) :
        m_vertices(vertices)
    {
        if(m_vertices.front() == m_vertices.back()) {
            opp_error("PolygonCondition vector must not contain the first point as end point");
        }
        if(m_vertices.size() <= 3) {
            opp_error("PolygonCondition vector must contain at least 3 points");
        }
    }

    /**
     * The function will return true if the TraCIMobility vehicle is inside the given polygon,
     * or false if its not.
     * \param TraCIMobility to check if its inside
     * \return result of the performed check
     */
    bool testCondition(Veins::TraCIMobility* car);

private:
    std::vector<Coord> m_vertices;

    int edges() const;

};

#endif /* _POLYGONCONDITION_H_ */
