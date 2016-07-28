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
    PolygonCondition(const std::vector<Coord>& vertices);

    /**
     * The function will return true if the TraCIMobility vehicle is inside the given polygon,
     * or false if its not.
     * \param TraCIMobility to check if its inside
     * \return result of the performed check
     */
    bool testCondition(const Vehicle& car);

private:
    std::vector<Coord> m_vertices;

    int edges() const;

};

#endif /* _POLYGONCONDITION_H_ */
