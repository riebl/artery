/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef SENSORCONFIGURATION_H_
#define SENSORCONFIGURATION_H_

#include "artery/envmod/sensor/SensorVisualizationConfig.h"
#include "artery/envmod/sensor/FieldOfView.h"
#include "artery/envmod/sensor/SensorPosition.h"
#include "artery/utility/Geometry.h"
#include <vector>

namespace artery
{

class EnvironmentModelObject;

struct SensorConfig
{
        std::string egoID; /*< ego ID of the vehicle carrying this sensor */
        unsigned int sensorID = 0; /*< uniqu sensor ID */
};

struct SensorConfigRadar : public SensorConfig
{
        FieldOfView fieldOfView;
        SensorPosition sensorPosition = SensorPosition::FRONT;
        unsigned numSegments = 0; /*< number of sensor cone segments (0 build a triangle) */
        bool doLineOfSightCheck = true; /*< false for simple "object in sensor cone" tests */
        SensorVisualizationConfig visualizationConfig;
};


/**
 * Creates sensor cone defined by at least 3 points:
 * sensor attachment point, left triangle point, right triangle point.
 *
 * Sensor range tests become more realistic with more configured segments.
 * @param config radar sensor configuration describing the cone geometry
 * @return polygon approximating cone
 */
std::vector<Position> createSensorArc(const SensorConfigRadar&, const Position&, const Angle&);
std::vector<Position> createSensorArc(const SensorConfigRadar&, const EnvironmentModelObject&);

} // namespace artery

#endif /* SENSORCONFIGURATION_H_ */
