/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ENVMOD_FRONTRADAR_H_
#define ENVMOD_FRONTRADAR_H_

#include "artery/envmod/sensor/RadarSensor.h"

namespace artery
{

class FrontRadar : public RadarSensor
{
public:
    void initialize() override;
};

} // namespace artery

#endif /* ENVMOD_FRONTRADAR_H_ */
