/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ENVMOD_RADARSENSOR_H_
#define ENVMOD_RADARSENSOR_H_

#include "artery/envmod/sensor/FovSensor.h"

namespace artery
{

class RadarSensor : public FovSensor
{
public:
    const std::string& getSensorCategory() const override;
};

} // namespace artery

#endif /* ENVMOD_RADARSENSOR_H_ */
