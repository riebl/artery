/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/Facilities.h"
#include "artery/application/Middleware.h"
#include "artery/envmod/sensor/RsuFovSensor.h"
#include "artery/networking/PositionProvider.h"

namespace artery
{

void RsuFovSensor::initialize()
{
    FovSensor::initialize();
    mFovHeading = Angle::from_degree(par("fovHeading"));
}

SensorDetection RsuFovSensor::createSensorCone() const
{
    SensorDetection detection;
    detection.sensorOrigin = getFacilities().get_const<PositionProvider>().getCartesianPosition();
    detection.sensorCone = createSensorArc(mFovConfig, detection.sensorOrigin, mFovHeading);
    return detection;
}

} // namespace artery

