/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/envmod/sensor/FrontRadar.h"

namespace artery
{

Define_Module(FrontRadar);

void FrontRadar::initialize()
{
    RadarSensor::initialize();
    mRadarConfig.fieldOfView.range = 80.0 * boost::units::si::meter;
    mRadarConfig.fieldOfView.angle = 60.0 * boost::units::degree::degrees;
    mRadarConfig.sensorPosition = SensorPosition::FRONT;
    mRadarConfig.numSegments = 10;
    mRadarConfig.egoID = getEgoId();
    mRadarConfig.sensorID = getId();
}

} // namespace artery
