/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/envmod/EnvironmentModelObject.h"
#include "artery/envmod/GlobalEnvironmentModel.h"
#include "artery/envmod/Geometry.h"
#include "artery/envmod/sensor/SensorConfiguration.h"
#include "artery/traci/Cast.h"
#include "artery/traci/ControllableVehicle.h"
#include "artery/utility/IdentityRegistry.h"
#include "traci/Core.h"
#include <boost/geometry/geometries/register/linestring.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <inet/common/ModuleAccess.h>
#include <algorithm>
#include <array>

using namespace omnetpp;


namespace artery
{

Define_Module(GlobalEnvironmentModel)

namespace {
const simsignal_t refreshSignal = cComponent::registerSignal("EnvironmentModel.refresh");
const simsignal_t traciInitSignal = cComponent::registerSignal("traci.init");
const simsignal_t traciCloseSignal = cComponent::registerSignal("traci.close");
const simsignal_t traciNodeAddSignal = cComponent::registerSignal("traci.node.add");
const simsignal_t traciNodeRemoveSignal = cComponent::registerSignal("traci.node.remove");
const simsignal_t traciNodeUpdateSignal = cComponent::registerSignal("traci.node.update");

template<typename RT>
typename RT::const_query_iterator
query_intersections(RT& rtree, const std::vector<Position>& area)
{
#if BOOST_VERSION >= 106000 && BOOST_VERSION < 106200
    // Boost versions 1.60 and 1.61 do not compile without copy
    geometry::Polygon area_copy;
    boost::geometry::convert(area, area_copy);
    auto predicate = boost::geometry::index::intersects(area_copy);
#else
    auto predicate = boost::geometry::index::intersects(area);
#endif
    return rtree.qbegin(predicate);
}

} // namespace

GlobalEnvironmentModel::GlobalEnvironmentModel()
{
}

GlobalEnvironmentModel::~GlobalEnvironmentModel()
{
    clear();
}

void GlobalEnvironmentModel::refresh()
{
    for (auto& object_kv : mObjects) {
        object_kv.second->update();
    }

    buildObjectRtree();

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
        for (const auto& object_kv : mObjects) {
            // we add only polygon figures, thus static_cast should be safe
            auto polygon = static_cast<cPolygonFigure*>(mDrawVehicles->getFigure(figureIndex));
            std::vector<cFigure::Point> points;
            for (const Position& pos : object_kv.second->getOutline()) {
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

    auto object = std::make_shared<EnvironmentModelObject>(vehicle, id);
    auto insertion = mObjects.emplace(object->getExternalId(), object);
    if (insertion.second) {
        auto box = boost::geometry::return_envelope<geometry::Box>(object->getOutline());
        mObjectRtree.insert(ObjectRtreeValue { std::move(box), object });
    }
    ASSERT(mObjects.size() == mObjectRtree.size());
    return insertion.second;
}

bool GlobalEnvironmentModel::addObstacle(const std::string& id, std::vector<Position> outline)
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
    struct envelope_maker
    {
        inline ObstacleRtreeValue operator()(const ObstacleDB::value_type& item) const
        {
            const auto& obstacle = item.second;
            auto box = boost::geometry::return_envelope<geometry::Box>(obstacle->getOutline());
            return ObstacleRtreeValue { std::move(box), obstacle };
        }
    };

    // bulk loading of obstacles for efficient packing
    mObstacleRtree = ObstacleRtree { mObstacles | boost::adaptors::transformed(envelope_maker()) };
}

void GlobalEnvironmentModel::buildObjectRtree()
{
    struct envelope_maker
    {
        inline ObjectRtreeValue operator()(const ObjectDB::value_type& obj_kv) const
        {
            const std::shared_ptr<EnvironmentModelObject>& obj = obj_kv.second;
            auto box = boost::geometry::return_envelope<geometry::Box>(obj->getOutline());
            return ObjectRtreeValue { std::move(box), obj };
        }
    };

    // use bulk loading for efficient packing
    mObjectRtree = ObjectRtree { mObjects | boost::adaptors::transformed(envelope_maker()) };
    mTainted = false;
}

bool GlobalEnvironmentModel::removeVehicle(const std::string& objectId)
{
    bool erased = mObjects.erase(objectId) > 0;
    mTainted |= erased; /*< pending object rtree update */
    return erased;
}

void GlobalEnvironmentModel::removeVehicles()
{
    mObjects.clear();
    mObjectRtree.clear();
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

void GlobalEnvironmentModel::initialize()
{
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
    return found != mObjects.end() ? found->second : nullptr;
}

std::shared_ptr<EnvironmentModelObstacle> GlobalEnvironmentModel::getObstacle(const std::string& obsId)
{
    auto found = mObstacles.find(obsId);
    return found != mObstacles.end() ? found->second : nullptr;
}

std::vector<std::shared_ptr<EnvironmentModelObject>>
GlobalEnvironmentModel::preselectObjects(const std::string& ego, const std::vector<Position>& area)
{
    ASSERT(!mTainted);

    boost::geometry::validity_failure_type failure;
    if (!boost::geometry::is_valid(area, failure)) {
        std::string error_msg =  boost::geometry::validity_failure_type_message(failure);
        throw omnetpp::cRuntimeError("preselection polygon is invalid: %s", error_msg.c_str());
    }

    std::vector<std::shared_ptr<EnvironmentModelObject>> objectsInSearchArea;
    ObjectRtree::const_query_iterator it = query_intersections(mObjectRtree, area);
    for (; it != mObjectRtree.qend(); ++it) {
        if (it->second->getExternalId() != ego) {
            objectsInSearchArea.push_back(it->second);
        }
    }
    return objectsInSearchArea;
}

std::vector<std::shared_ptr<EnvironmentModelObstacle>>
GlobalEnvironmentModel::preselectObstacles(const std::vector<Position>& area)
{
    boost::geometry::validity_failure_type failure;
    if (!boost::geometry::is_valid(area, failure)) {
        std::string error_msg =  boost::geometry::validity_failure_type_message(failure);
        throw omnetpp::cRuntimeError("preselection polygon is invalid: %s", error_msg.c_str());
    }

    std::vector<std::shared_ptr<EnvironmentModelObstacle>> obstacles;
    ObstacleRtree::const_query_iterator it = query_intersections(mObstacleRtree, area);
    for (; it != mObstacleRtree.qend(); ++it) {
        obstacles.push_back(it->second);
    }
    return obstacles;
}

} // namespace artery
