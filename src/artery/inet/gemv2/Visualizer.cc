#include "artery/inet/gemv2/Visualizer.h"
#include "artery/inet/gemv2/ObstacleIndex.h"
#include "artery/inet/gemv2/VehicleIndex.h"

namespace artery
{
namespace gemv2
{

Define_Module(Visualizer)

void Visualizer::initialize(int stage)
{
    if (stage == 0) {
        mObstacleGroup = new omnetpp::cGroupFigure("obstacles");
        mVehicleGroup = new omnetpp::cGroupFigure("vehicles");

        auto canvas = this->getCanvas();
        canvas->addFigure(mObstacleGroup);
        canvas->addFigure(mVehicleGroup);
        canvas->setBackgroundColor(omnetpp::cFigure::GREY);
    }
}

void Visualizer::drawObstacles(const ObstacleIndex* index)
{
    for (int i = mObstacleGroup->getNumFigures() - 1; i >= 0; --i)
    {
        delete mObstacleGroup->removeFigure(i);
    }

    auto obstacles = index->getObstacles();
    for (auto& obstacle : obstacles)
    {
        auto polygon = new omnetpp::cPolygonFigure();
        for (const Position& pos : obstacle.getOutline())
        {
            polygon->addPoint(omnetpp::cFigure::Point { pos.x.value(), pos.y.value() });
        }
        mObstacleGroup->addFigure(polygon);
    }
}

void Visualizer::drawVehicles(const VehicleIndex* index)
{
    auto vehicles = index->getVehicles();

    // remove vehicles that do not exist longer
    for (auto it = mVehiclePolygons.begin(); it != mVehiclePolygons.end();)
    {
        auto found = vehicles.find(it->first);
        if (found != vehicles.end()) {
            ++it;
        } else {
            delete it->second->removeFromParent();
            it = mVehiclePolygons.erase(it);
        }
    }

    for (auto& name_vehicle : vehicles)
    {
        auto found = mVehiclePolygons.find(name_vehicle.first);
        if (found == mVehiclePolygons.end()) {
            // insert new vehicle polygon
            omnetpp::cPolygonFigure* polygon = new omnetpp::cPolygonFigure(name_vehicle.first.c_str());
            mVehicleGroup->addFigure(polygon);
            mVehiclePolygons[name_vehicle.first] = polygon;
            for (const Position& pos : name_vehicle.second.getOutline())
            {
                polygon->addPoint(omnetpp::cFigure::Point { pos.x.value(), pos.y.value() });
            }
            polygon->setLineColor(omnetpp::cFigure::BLUE);
        } else {
            // update existing polygon
            omnetpp::cPolygonFigure* polygon = found->second;
            auto& outline = name_vehicle.second.getOutline();
            for (int i = 0; i < polygon->getNumPoints(); ++i)
            {
                polygon->setPoint(i, omnetpp::cFigure::Point { outline[i].x.value(), outline[i].y.value() });
            }
        }
    }
}

} // namespace gemv2
} // namespace artery
