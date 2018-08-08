/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef LOCALENVIRONMENTMODEL_H_
#define LOCALENVIRONMENTMODEL_H_

#include <boost/iterator/filter_iterator.hpp>
#include <boost/range/iterator_range.hpp>
#include <omnetpp/clistener.h>
#include <omnetpp/csimplemodule.h>
#include <omnetpp/simtime.h>
#include <functional>
#include <map>
#include <memory>
#include <vector>

namespace artery
{

class EnvironmentModelObject;
class GlobalEnvironmentModel;
class Middleware;
class Sensor;
class SensorDetection;

/**
 * Local representation of the global environment model
 *
 * LocalEnvironmentModel tracks the GlobalEnvironmentModel's objects
 * visible by the local sensors feeding this local model.
 */
class LocalEnvironmentModel : public omnetpp::cSimpleModule, public omnetpp::cListener
{
public:
    using Object = std::weak_ptr<EnvironmentModelObject>;

    class TrackingTime
    {
    public:
        TrackingTime();
        void tap();

        omnetpp::SimTime first() const { return mFirst; }
        omnetpp::SimTime last() const { return mLast; }

    private:
        omnetpp::SimTime mFirst;
        omnetpp::SimTime mLast;
    };

    class Tracking
    {
    public:
        using TrackingMap = std::map<const Sensor*, TrackingTime>;

        Tracking(const Sensor* sensor);

        bool expired() const;
        void update();
        void tap(const Sensor*);

        const TrackingMap& sensors() const { return mSensors; }

    private:
        TrackingMap mSensors;
    };

    using TrackedObjects = std::map<Object, Tracking, std::owner_less<Object>>;
    using TrackedObject = typename TrackedObjects::value_type;


    LocalEnvironmentModel();
    virtual ~LocalEnvironmentModel() = default;

    int numInitStages() const override;
    void initialize(int stage) override;
    void finish() override;
    void receiveSignal(cComponent*, omnetpp::simsignal_t, cObject*, cObject*) override;

    /**
     * Updates the local environment model
     *
     * This method is supposed to get called at each TraCI simulation step.
     * Expired and no longer existing objects are removed from the local tracking.
     */
    void update();

    /**
     * Complements the local database with the sensor data objects
     * @param objs Sensor detection result including objects and obstacles
     * @param sensor Detections are measured by this sensor
     */
    void complementObjects(const SensorDetection&, const Sensor&);

    /**
     * Get all currently seen objects by any local sensor
     */
    const TrackedObjects& allObjects() const { return mObjects; }

private:
    void initializeSensors();

    Middleware* mMiddleware;
    GlobalEnvironmentModel* mGlobalEnvironmentModel;
    TrackedObjects mObjects;
    std::vector<Sensor*> mSensors;
};

using TrackedObjectsFilterPredicate = std::function<bool(const LocalEnvironmentModel::TrackedObject&)>;
using TrackedObjectsFilterIterator = boost::filter_iterator<TrackedObjectsFilterPredicate, LocalEnvironmentModel::TrackedObjects::const_iterator>;
using TrackedObjectsFilterRange = boost::iterator_range<TrackedObjectsFilterIterator>;

TrackedObjectsFilterRange filterBySensorCategory(const LocalEnvironmentModel::TrackedObjects&, const std::string&);

} // namespace artery

#endif /* LOCALENVIRONMENTMODEL_H_ */
