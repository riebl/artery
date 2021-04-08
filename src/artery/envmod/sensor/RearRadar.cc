/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/envmod/sensor/RearRadar.h"

namespace artery
{

Define_Module(RearRadar)

void RearRadar::initialize()
{
    RadarSensor::initialize();
    mRadarConfig.sensorPosition = SensorPosition::BACK;
    mRadarConfig.egoID = getEgoId();
    mRadarConfig.sensorID = getId();
}

} // namespace artery
