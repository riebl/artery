/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/envmod/EnvironmentModelObject.h"
#include "artery/envmod/GlobalEnvironmentModel.h"
#include "artery/envmod/Geometry.h"
#include "artery/envmod/InterdistanceMatrix.h"
#include "artery/envmod/PreselectionPolygon.h"
#include "artery/envmod/PreselectionRtree.h"
#include "artery/envmod/sensor/SensorConfiguration.h"
#include "artery/traci/Cast.h"
#include "artery/traci/ControllableVehicle.h"
#include "artery/utility/IdentityRegistry.h"
#include "traci/Core.h"
#include <boost/geometry/geometries/register/linestring.hpp>
#include <inet/common/ModuleAccess.h>
#include <algorithm>
#include <array>

using namespace omnetpp;


namespace artery
{

namespace {
const simsignal_t refreshSignal = cComponent::registerSignal("EnvironmentModel.refresh");
const simsignal_t traciInitSignal = cComponent::registerSignal("traci.init");
const simsignal_t traciCloseSignal = cComponent::registerSignal("traci.close");
const simsignal_t traciNodeAddSignal = cComponent::registerSignal("traci.node.add");
const simsignal_t traciNodeRemoveSignal = cComponent::registerSignal("traci.node.remove");
const simsignal_t traciNodeUpdateSignal = cComponent::registerSignal("traci.node.update");
}

Define_Module(GlobalEnvironmentModel)


GlobalEnvironmentModel::GlobalEnvironmentModel()
{
}

GlobalEnvironmentModel::~GlobalEnvironmentModel()
{
    clear();
}

void GlobalEnvironmentModel::refresh()
{
    for (auto& object : mObjects) {
        object->update();
    }

    mPreselector->update();
    mTainted = false;

    if (mDrawVehicles) {
        int numObjects = mObjects.size();
        int numFigures = mDrawVehicles->getNumFigures();

        // add missing polygon figures
        while (numFigures < numObjects) {
            auto polygon = new cPolygonFigure();
            polygon->setFillColor(cFigure::BLUE);
            polygon->setFilled(true);
            mDrawVehicles->addFigure(polygon);
            ++numFigures;
        }

        // remove excessive polygon figures
        while (numFigures > numObjects) {
            --numFigures;
            delete mDrawVehicles->removeFigure(numFigures);
        }

        // update figures with current outlines
        int figureIndex = 0;
        for (const auto& object : mObjects) {
            // we add only polygon figures, thus static_cast should be safe
            auto polygon = static_cast<cPolygonFigure*>(mDrawVehicles->getFigure(figureIndex));
            std::vector<cFigure::Point> points;
            for (const Position& pos : object->getOutline()) {
                points.push_back(cFigure::Point { pos.x.value(), pos.y.value() });
            }
            polygon->setPoints(points);
            ++figureIndex;
        }
    }

    emit(refreshSignal, this);
}

bool GlobalEnvironmentModel::addVehicle(traci::VehicleController* vehicle)
{
    uint32_t id = 0;
    if (mIdentityRegistry) {
        auto identity = mIdentityRegistry->lookup<IdentityRegistry::traci>(vehicle->getVehicleId());
        if (identity) {
            id = identity->application;
        }
    }

    auto insertion = mObjects.insert(std::make_shared<EnvironmentModelObject>(vehicle, id));
    mTainted |= insertion.second; /*< pending preselector update if insertion took place */
    return insertion.second;
}

bool GlobalEnvironmentModel::addObstacle(std::string id, std::vector<Position> outline)
{
    boost::geometry::correct(outline);
    std::string invalid;
    if (!boost::geometry::is_valid(outline, invalid)) {
        // skip self-intersecting geometry
        EV_ERROR << "skip invalid obstacle polygon " << id << ": " << invalid << " \n";
        return false;
    }
    auto insertion = mObstacles.emplace(id, std::make_shared<EnvironmentModelObstacle>(id, outline));

    if (mDrawObstacles) {
        auto polygon = new cPolygonFigure();
        polygon->setFilled(true);
        polygon->setFillColor(cFigure::BLACK);
        polygon->setFillOpacity(0.7);
        for (const Position& pos : outline) {
            polygon->addPoint(cFigure::Point { pos.x.value(), pos.y.value() });
        }
        mDrawObstacles->addFigure(polygon);
    }

    return insertion.second;
}

void GlobalEnvironmentModel::buildObstacleRtree()
{
    std::string message;
    for (const auto& obstacle_kv : mObstacles) {
        auto& obstacle = obstacle_kv.second;
        const auto& polygon = obstacle->getOutline();
        if (boost::geometry::is_valid(polygon, message)) {
            auto bb = boost::geometry::return_envelope<geometry::Box>(polygon);
            mObstacleRtree.insert(std::make_pair(bb, obstacle->getObstacleId()));
        } else {
            throw std::runtime_error("invalid obstacle polygon #" + obstacle->getObstacleId() + " : " + message);
        }
    }
}

bool GlobalEnvironmentModel::removeVehicle(std::string objID)
{
    mTainted = true; /*< pending preselector update */
    return mObjects.erase(objID) > 0;
}

void GlobalEnvironmentModel::removeVehicles()
{
    mObjects.clear();
    mPreselector->update();
    mTainted = false;

    if (mDrawVehicles) {
        // remove all polygons
        for (int i = mDrawVehicles->getNumFigures() - 1; i >= 0; --i) {
            delete mDrawVehicles->removeFigure(i);
        }
    }
}

void GlobalEnvironmentModel::clear()
{
    removeVehicles();
    mObstacles.clear();
    mObstacleRtree.clear();
}

SensorDetection GlobalEnvironmentModel::detectObjects(std::function<SensorDetection(ObstacleRtree&, PreselectionMethod&)> detect)
{
    ASSERT(!mTainted); /*< object database and preselector are in sync */
    return detect(mObstacleRtree, *mPreselector);
}

void GlobalEnvironmentModel::initialize()
{
    const int preselectionMethod = par("preselectionMethod");
    switch (preselectionMethod)
    {
    case 1:
        EV_INFO << "envmod: Preselection by interdistance matrix\n";
        mPreselector.reset(new InterdistanceMatrix(mObjects));
        break;

    case 2:
        EV_INFO << "envmod: Preselection by polygon intersection tests\n";
        mPreselector.reset(new PreselectionPolygon(mObjects));
        break;

    case 3:
        EV_INFO << "envmod: Preselection by RTree\n";
        mPreselector.reset(new PreselectionRtree(mObjects));
        break;

    default:
        throw cRuntimeError("Unknown preselection method");
    }

    cModule* traci = getModuleByPath(par("traciModule"));
    if (traci) {
        traci->subscribe(traciInitSignal, this);
        traci->subscribe(traciCloseSignal, this);

        traci->subscribe(traciNodeAddSignal, this);
        traci->subscribe(traciNodeRemoveSignal, this);
        traci->subscribe(traciNodeUpdateSignal, this);
    } else {
        throw cRuntimeError("No TraCI module found for signal subscription");
    }

    mIdentityRegistry = inet::findModuleFromPar<IdentityRegistry>(par("identityRegistryModule"), this);
    mTainted = false;

    if (par("drawObstacles")) {
        mDrawObstacles = new omnetpp::cGroupFigure("obstacles");
        getCanvas()->addFigure(mDrawObstacles);
    }

    if (par("drawVehicles")) {
        mDrawVehicles = new omnetpp::cGroupFigure("vehicles");
        getCanvas()->addFigure(mDrawVehicles);
    }

    std::string obstacleTypes = par("obstacleTypes");
    boost::split(mObstacleTypes, obstacleTypes, boost::is_any_of(" "));
}

void GlobalEnvironmentModel::finish()
{
    removeVehicles();
}

void GlobalEnvironmentModel::receiveSignal(cComponent* source, simsignal_t signal, const SimTime&, cObject*)
{
    if (signal == traciInitSignal) {
        auto core = check_and_cast<traci::Core*>(source);
        fetchObstacles(*core->getAPI());
    } else if (signal == traciCloseSignal) {
        clear();
    }
}

void GlobalEnvironmentModel::receiveSignal(cComponent*, simsignal_t signal, const char* id, cObject* obj)
{
    if (signal == traciNodeAddSignal) {
        cModule* module = dynamic_cast<cModule*>(obj);
        if (module) {
            auto controller = getVehicleController(module);
            if (controller) {
                addVehicle(controller);
            }
        }
    } else if (signal == traciNodeRemoveSignal) {
        removeVehicle(id);
    }
}

void GlobalEnvironmentModel::receiveSignal(cComponent*, simsignal_t signal, unsigned long nodes, cObject* obj)
{
    if (signal == traciNodeUpdateSignal) {
        refresh();
    }
}

void GlobalEnvironmentModel::fetchObstacles(const traci::API& traci)
{
    auto& polygons = traci.polygon;
    const traci::Boundary boundary { traci.simulation.getNetBoundary() };
    for (const std::string& id : polygons.getIDList()) {
        if (!mObstacleTypes.empty()) {
            std::string type = polygons.getType(id);
            if (mObstacleTypes.find(type) == mObstacleTypes.end()) {
                // skip polygon because its type is not in our filter set
                EV_DEBUG << "ignore polygon " << id << " of type " << type << "\n";
                continue;
            }
        }

        std::vector<Position> shape;
        for (const traci::TraCIPosition& traci_point : polygons.getShape(id).value) {
            shape.push_back(traci::position_cast(boundary, traci_point));
        }
        if (shape.size() >= 3) {
            addObstacle(id, shape);
        } else {
            EV_WARN << "skip obstacle polygon " << id << " because its shape is degraded\n";
        }
    }

    buildObstacleRtree();
}

traci::VehicleController* GlobalEnvironmentModel::getVehicleController(cModule* module)
{
    assert(module);
    auto vehicle = dynamic_cast<ControllableVehicle*>(module->getModuleByPath(par("nodeMobilityModule")));
    return vehicle ? vehicle->getVehicleController() : nullptr;
}

std::shared_ptr<EnvironmentModelObject> GlobalEnvironmentModel::getObject(const std::string& objId)
{
    auto found = mObjects.find(objId);
    return found != mObjects.end() ? *found : nullptr;
}

std::shared_ptr<EnvironmentModelObstacle> GlobalEnvironmentModel::getObstacle(const std::string& obsId)
{
    auto found = mObstacles.find(obsId);
    return found != mObstacles.end() ? found->second : nullptr;
}

} // namespace artery
