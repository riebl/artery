/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ENVMOD_REARRADAR_H_
#define ENVMOD_REARRADAR_H_

#include "artery/envmod/sensor/RadarSensor.h"

namespace artery
{

class RearRadar : public RadarSensor
{
public:
    void initialize() override;
};

} // namespace artery

#endif /* ENVMOD_REARRADAR_H_ */
