/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/envmod/service/EnvmodPrinter.h"
#include "artery/envmod/EnvironmentModelObject.h"
#include "artery/envmod/LocalEnvironmentModel.h"
#include "artery/envmod/sensor/Sensor.h"
#include "artery/traci/VehicleController.h"
#include <boost/units/io.hpp>
#include <omnetpp/clog.h>

namespace artery
{

Define_Module(EnvmodPrinter)

void EnvmodPrinter::initialize()
{
    ItsG5Service::initialize();
    mLocalEnvironmentModel = getFacilities().get_mutable_ptr<LocalEnvironmentModel>();
    mEgoId = getFacilities().get_const<traci::VehicleController>().getVehicleId();
}

void EnvmodPrinter::trigger()
{
    Enter_Method("trigger");
    auto& allObjects = mLocalEnvironmentModel->allObjects();

    EV_DETAIL << mEgoId << "--- By category ---" << std::endl;
    printSensorObjectList("Radar Sensor Object List", filterBySensorCategory(allObjects, "Radar"));
    printSensorObjectList("CAM Sensor Object List", filterBySensorCategory(allObjects, "CA"));

    EV_DETAIL << mEgoId << "--- By name ---" << std::endl;
    for (auto &sensor: mLocalEnvironmentModel->getSensors()) {
        std::string sensorName = sensor->getSensorName();
        printSensorObjectList(sensorName + " Object List", filterBySensorName(allObjects, sensorName));
    }
}

void EnvmodPrinter::printSensorObjectList(const std::string& title, const TrackedObjectsFilterRange& objs)
{
    EV_DETAIL << mEgoId << "--- " << title << " (" << boost::size(objs) << " objects) ---" << std::endl;

    for (const auto& obj : objs)
    {
        const auto obj_ptr = obj.first.lock();
        if (!obj_ptr) {
            continue; /*< objects remain in tracking briefly after leaving simulation */
        }
        const auto& vd = obj_ptr->getVehicleData();
        EV_DETAIL
            << "station ID: " << vd.station_id()
            << " TraCI ID: " << obj_ptr->getExternalId()
            << " lon: " << vd.longitude()
            << " lat: " << vd.latitude()
            << " speed: " << vd.speed()
            << " when: " << vd.updated()
            << std::endl;
    }
}

} // namespace artery
