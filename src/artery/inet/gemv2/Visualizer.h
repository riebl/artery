#ifndef ARTERY_GEMV2_VISUALIZER_H_U4LMLGVJ
#define ARTERY_GEMV2_VISUALIZER_H_U4LMLGVJ

#include "artery/utility/Geometry.h"
#include <omnetpp/ccanvas.h>
#include <omnetpp/csimplemodule.h>
#include <string>
#include <unordered_map>

namespace artery
{
namespace gemv2
{

// forward declarations
class ObstacleIndex;
class VehicleIndex;

class Visualizer : public omnetpp::cSimpleModule
{
public:
    void initialize(int stage) override;

    void drawObstacles(const ObstacleIndex*);
    void drawVehicles(const VehicleIndex*);
    void drawReflectionRays(const Position&, const Position&,
            const std::vector<Position>&, const std::vector<Position>&);
    void drawDiffractionRays(const Position&, const Position&, const std::vector<Position>&);
    void drawFoliageRay(const Position&, const Position&, const std::vector<Position>&);

protected:
    omnetpp::cGroupFigure* getObstacleGroup(const omnetpp::cModule*);
    void drawRays(const Position&, const Position&, const std::vector<Position>&, omnetpp::cFigure::Color) const;

private:
    omnetpp::cGroupFigure* mVehicleGroup;
    omnetpp::cGroupFigure* mRaysGroup;
    std::unordered_map<std::string, omnetpp::cPolygonFigure*> mVehiclePolygons;
    std::unordered_map<int, omnetpp::cGroupFigure*> mObstacleGroups;

    omnetpp::cFigure::Color mBackgroundColor;
    omnetpp::cFigure::Color mDiffractionColor;
    omnetpp::cFigure::Color mFoliageColorInside;
    omnetpp::cFigure::Color mFoliageColorOutside;
    omnetpp::cFigure::Color mReflectionColorObstacle;
    omnetpp::cFigure::Color mReflectionColorVehicle;
};

} // namespace gemv2
} // namespace artery

#endif /* ARTERY_GEMV2_VISUALIZER_H_U4LMLGVJ */

