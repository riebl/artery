#ifndef ARTERY_GEMV2_VISUALIZER_H_U4LMLGVJ
#define ARTERY_GEMV2_VISUALIZER_H_U4LMLGVJ

#include <omnetpp/ccanvas.h>
#include <omnetpp/csimplemodule.h>
#include <string>
#include <unordered_map>

namespace artery
{
namespace gemv2
{

class ObstacleIndex;
class VehicleIndex;

class Visualizer : public omnetpp::cSimpleModule
{
public:
    void initialize(int stage) override;

    void drawObstacles(const ObstacleIndex*);
    void drawVehicles(const VehicleIndex*);

private:
    omnetpp::cGroupFigure* mObstacleGroup;
    omnetpp::cGroupFigure* mVehicleGroup;
    std::unordered_map<std::string, omnetpp::cPolygonFigure*> mVehiclePolygons;
};

} // namespace gemv2
} // namespace artery

#endif /* ARTERY_GEMV2_VISUALIZER_H_U4LMLGVJ */

