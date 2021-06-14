/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef GLOBALENVIRONMENTMODEL_H_
#define GLOBALENVIRONMENTMODEL_H_


// #include "artery/envmod/sensor/Sensor.h"
#include "artery/envmod/sensor/SensorDetection.h"
#include "artery/envmod/Geometry.h"
#include "artery/envmod/EnvironmentModelObject.h"
#include "artery/envmod/EnvironmentModelObstacle.h"
#include "artery/utility/Geometry.h"
#include <omnetpp/ccanvas.h>
#include <omnetpp/clistener.h>
#include <omnetpp/csimplemodule.h>
#include <boost/geometry/index/rtree.hpp>
#include <unordered_map>
#include <memory>
#include <string>


namespace traci {
    class API;
    class VehicleController;
}

namespace artery
{

class EnvironmentModelObstacle;
class IdentityRegistry;

/**
 * Implementation of the environment model.
 */
class GlobalEnvironmentModel : public omnetpp::cSimpleModule, public omnetpp::cListener
{
public:
    GlobalEnvironmentModel();
    virtual ~GlobalEnvironmentModel();

    // cSimpleModule life-cycle
    void initialize() override;
    void finish() override;

    // cListener handlers
    void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, const omnetpp::SimTime&, omnetpp::cObject*) override;
    void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, const char*, omnetpp::cObject*) override;
    void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, unsigned long, omnetpp::cObject*) override;

    /**
     * Fetch an object by its external id.
     * @param externalID
     * @return model object matching external id
     */
    std::shared_ptr<EnvironmentModelObject> getObject(const std::string& objId);

    /**
     * Get an obstacle by its id
     * @param obsId obstacle id
     * @return obstacle model matching the id or nullptr
     */
    std::shared_ptr<EnvironmentModelObstacle> getObstacle(const std::string& obsId);

    /**
     * Preselect all objects close to the given area
     * @param ego identifier of the ego object, which is filtered out of the result
     * @param area search polygon
     * @return preselected objects, i.e. candidates for precise sensor checks
     */
    std::vector<std::shared_ptr<EnvironmentModelObject>>
    preselectObjects(const std::string& ego, const std::vector<Position>& area);

    /**
     * Preselect all obstacles close to the given area
     * @param area search polygon
     * @return preselected obstacles
     */
    std::vector<std::shared_ptr<EnvironmentModelObstacle>>
    preselectObstacles(const std::vector<Position>& area);

private:
    /**
     * Refresh all dynamic objects in the database.
     */
    void refresh();

    /**
     * Add object to the environment database
     * @param vehicle TraCI mobility corresponding to vehicle
     * @return true if successful
     */
    bool addObject(traci::Controller* object);

    /**
     * Remove vehicle from the database
     * @param nodeId TraCI id of vehicle to be removed
     * @return true if the vehicle is successfully removed
     */
    bool removeObject(const std::string& id);

    /**
     * Remove all known vehicles from internal database
     */
    void removeObjects();

    /**
     * Add (static) obstacles to the obstacle database
     * @param id Obstacle's id
     * @param outline Obstacle's outline
     * @return true if it could be added
     */
    bool addObstacle(const std::string& id, std::vector<Position> outline);

    /**
     * Create the obstacle rtree.
     * This method should be called after all static obstacles have been added.
     */
    void buildObstacleRtree();

    /**
     * Create the object rtree.
     */
    void buildObjectRtree();

    /**
     * Clears the internal database completely
     */
    void clear();

    /**
     * Fetch static obstacles (polygons) from TraCI
     * @param api TraCI API object
     */
    void fetchObstacles(const traci::API& api);

    /**
     * Try to get controller corresponding to given module
     * @param mod host module
     * @return nullptr if no controller is available
     */
    virtual traci::Controller* getController(omnetpp::cModule* mod);

    using ObjectDB = std::unordered_map<std::string, std::shared_ptr<EnvironmentModelObject>>;
    using ObjectRtreeValue = std::pair<geometry::Box, std::shared_ptr<EnvironmentModelObject>>;
    using ObjectRtree = boost::geometry::index::rtree<ObjectRtreeValue, boost::geometry::index::quadratic<16>>;
    using ObstacleDB = std::unordered_map<std::string, std::shared_ptr<EnvironmentModelObstacle>>;
    using ObstacleRtreeValue = std::pair<geometry::Box, std::shared_ptr<EnvironmentModelObstacle>>;
    using ObstacleRtree = boost::geometry::index::rtree<ObstacleRtreeValue, boost::geometry::index::rstar<16>>;

    ObjectDB mObjects;
    ObjectRtree mObjectRtree;
    ObstacleDB mObstacles;
    ObstacleRtree mObstacleRtree;
    IdentityRegistry* mIdentityRegistry;
    bool mTainted = false;
    omnetpp::cGroupFigure* mDrawObstacles = nullptr;
    omnetpp::cGroupFigure* mDrawVehicles = nullptr;
    std::set<std::string> mObstacleTypes;
};

} // namespace artery

#endif /* GLOBALENVIRONMENTMODEL_H_ */
