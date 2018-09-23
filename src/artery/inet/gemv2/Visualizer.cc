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
        mVehicleGroup = new omnetpp::cGroupFigure("vehicles");
        mRaysGroup = new omnetpp::cGroupFigure("rays");

        auto canvas = this->getCanvas();
        canvas->addFigure(mVehicleGroup);
        canvas->addFigure(mRaysGroup);
        canvas->setBackgroundColor(omnetpp::cFigure::GREY);
    }
}

omnetpp::cGroupFigure* Visualizer::getObstacleGroup(const omnetpp::cModule* module)
{
    omnetpp::cGroupFigure* figure = nullptr;
    auto found = mObstacleGroups.find(module->getId());
    if (found != mObstacleGroups.end()) {
        figure = found->second;
    } else {
        figure = new omnetpp::cGroupFigure(module->getName());
        this->getCanvas()->addFigure(figure);
        mObstacleGroups.emplace(module->getId(), figure);
    }

    return figure;
}

void Visualizer::drawObstacles(const ObstacleIndex* index)
{
    omnetpp::cGroupFigure* group = getObstacleGroup(index);
    for (int i = group->getNumFigures() - 1; i >= 0; --i)
    {
        delete group->removeFigure(i);
    }

    auto obstacles = index->getObstacles();
    for (auto& obstacle : obstacles)
    {
        auto polygon = new omnetpp::cPolygonFigure();
        polygon->setLineColor(index->getColor());
        for (const Position& pos : obstacle.getOutline())
        {
            polygon->addPoint(omnetpp::cFigure::Point { pos.x.value(), pos.y.value() });
        }
        group->addFigure(polygon);
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

    // remove all previous rays
    for (int i = mRaysGroup->getNumFigures() - 1; i >= 0; --i)
    {
        delete mRaysGroup->removeFigure(i);
    }
}

void Visualizer::drawReflectionRays(const Position& tx, const Position& rx,
        const std::vector<Position>& obstacles, const std::vector<Position>& vehicles)
{
    drawRays(tx, rx, obstacles, omnetpp::cFigure::RED);
    drawRays(tx, rx, vehicles, omnetpp::cFigure::MAGENTA);
}

void Visualizer::drawDiffractionRays(const Position& tx, const Position& rx, const std::vector<Position>& corners)
{
    static const omnetpp::cFigure::Color color("darkgreen");
    drawRays(tx, rx, corners, color);
}

void Visualizer::drawRays(const Position& tx, const Position& rx,
        const std::vector<Position>& points, omnetpp::cFigure::Color c) const
{
    const omnetpp::cFigure::Point begin { tx.x.value(), tx.y.value() };
    const omnetpp::cFigure::Point end { rx.x.value(), rx.y.value() };

    for (auto& point : points)
    {
        auto figure = new omnetpp::cPolylineFigure();
        mRaysGroup->addFigure(figure);
        figure->setLineColor(c);
        figure->addPoint(begin);
        figure->addPoint(omnetpp::cFigure::Point { point.x.value(), point.y.value() });
        figure->addPoint(end);
    }
}

} // namespace gemv2
} // namespace artery
