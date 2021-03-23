/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/envmod/sensor/RadarSensor.h"

namespace artery
{
Define_Module(RadarSensor);

void RadarSensor::initialize()
{
    FovSensor::initialize();
    mFovConfig.egoID = getEgoId();
    mFovConfig.sensorID = getId();
    mFovConfig.sensorPosition = sensorPositionStrings.find(par("attachmentPoint"))->second;
}

} // namespace artery
