/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/envmod/sensor/RsuRadarSensor.h"

namespace artery
{

Define_Module(RsuRadarSensor);

const std::string& RsuRadarSensor::getSensorCategory() const
{
    static const std::string category = "Radar";
    return category;
}

} // namespace artery

