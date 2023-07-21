#ifndef ARTERY_POLYGONCONDITION_H_
#define ARTERY_POLYGONCONDITION_H_

#include "artery/storyboard/Condition.h"
#include "artery/utility/Geometry.h"
#include <omnetpp/ccanvas.h>
#include <vector>

namespace artery
{

class STORYBOARD_API PolygonCondition : public Condition
{
public:
    /**
     * Creates a polygon area from the given Omnet++ Coordinates
     * \note The vector must not contain the first point as end point
     */
    PolygonCondition(const std::vector<Position>& vertices);

    /**
     * The function will return true if the TraCI vehicle is inside the given polygon,
     * or false if its not.
     * \param Vehicle to check if its inside
     * \return result of the performed check
     */
    ConditionResult testCondition(const Vehicle& car);

    virtual void drawCondition(omnetpp::cCanvas*) override;

private:
    std::vector<Position> m_vertices;
    bool mDraw;
    int STORYBOARD_LOCAL edges() const;
};

} // namespace artery

#endif /* ARTERY_POLYGONCONDITION_H_ */
