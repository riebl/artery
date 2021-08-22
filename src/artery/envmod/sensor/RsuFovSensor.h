/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ENVMOD_RSUFOVSENSOR_H_
#define ENVMOD_RSUFOVSENSOR_H_

#include "artery/envmod/sensor/FovSensor.h"

namespace artery
{

class RsuFovSensor : public FovSensor
{
protected:
    void initialize() override;
    SensorDetection createSensorCone() const override;

    Angle mFovHeading;
};

} // namespace artery

#endif /* ENVMOD_RSUFOVSENSOR_H_ */
