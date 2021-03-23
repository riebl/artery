/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/envmod/PreselectionPolygon.h"
#include "artery/envmod/sensor/SensorConfiguration.h"
#include <boost/geometry.hpp>
#include <omnetpp/cexception.h>

namespace artery
{

void PreselectionPolygon::update()
{
}

std::vector<std::string> PreselectionPolygon::select(const EnvironmentModelObject& ego, const SensorConfigFov& config)
{
    const auto& sensorCone = createSensorArc(config, ego);
    if (!boost::geometry::is_valid(sensorCone)) {
        throw omnetpp::cRuntimeError("polygon of sensor cone is invalid");
    }

    std::vector<std::string> objectsInSensorCone;
    for (const auto& object : mObjects) {
        if (object->getExternalId() != ego.getExternalId()) {
            if (boost::geometry::intersects(object->getOutline(), sensorCone)) {
                objectsInSensorCone.push_back(object->getExternalId());
            }
        }
    }
    return objectsInSensorCone;
}

} // namespace artery
