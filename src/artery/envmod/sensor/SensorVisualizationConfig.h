/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ENVMOD_SENSORVISUALIZATIONCONFIG_H_
#define ENVMOD_SENSORVISUALIZATIONCONFIG_H_

// forward declaration
namespace omnetpp { class cXMLElement; }

namespace artery
{

struct SensorVisualizationConfig
{
    SensorVisualizationConfig();
    SensorVisualizationConfig(const omnetpp::cXMLElement*);

    bool sensorCone; //!< Shows the sensor cone polygon
    bool obstaclesInSensorRange; //!< Shows the obstacle polygons in sensor range
    bool objectsInSensorRange; //!< Shows the object polygons
    bool linesOfSight; //!< Shows the lines of sight (line from ego to every visible point of a vehicle)
};

} // namespace artery

#endif /* ENVMOD_SENSORVISUALIZATIONCONFIG_H_ */
