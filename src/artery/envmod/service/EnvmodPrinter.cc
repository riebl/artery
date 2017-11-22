/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/envmod/service/EnvmodPrinter.h"
#include "artery/envmod/EnvironmentModelObject.h"
#include "artery/envmod/LocalEnvironmentModel.h"
#include "artery/traci/VehicleController.h"
#include <boost/units/io.hpp>
#include <omnetpp/clog.h>

namespace artery
{

Define_Module(EnvmodPrinter);

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
    printSensorObjectList("Radar Sensor Object List", filterBySensorCategory(allObjects, "Radar"));
    printSensorObjectList("CAM Sensor Object List", filterBySensorCategory(allObjects, "CA"));
}

void EnvmodPrinter::printSensorObjectList(const std::string& title, const TrackedObjectsFilterRange& objs)
{
    EV_DETAIL << mEgoId << "--- " << title << " (" << boost::size(objs) << " objects) ---" << std::endl;

    for (const auto& obj : objs)
    {
        std::weak_ptr<EnvironmentModelObject> obj_ptr = obj.first;
        if (obj_ptr.expired()) continue; /*< objects remain in tracking briefly after leaving simulation */
        const auto& vd = obj_ptr.lock()->getVehicleData();
        EV_DETAIL
            << "station ID: " << vd.station_id()
            << " lon: " << vd.longitude()
            << " lat: " << vd.latitude()
            << " speed: " << vd.speed()
            << " when: " << vd.updated()
            << std::endl;
    }
}

} // namespace artery
