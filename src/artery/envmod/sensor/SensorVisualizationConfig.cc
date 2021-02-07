#include "artery/envmod/sensor/SensorVisualizationConfig.h"
#include <omnetpp/cxmlelement.h>
#include <cstring>

using namespace omnetpp;

namespace artery
{

SensorVisualizationConfig::SensorVisualizationConfig() :
    sensorCone(false), obstaclesInSensorRange(false),
    objectsInSensorRange(false), linesOfSight(false)
{
}

SensorVisualizationConfig::SensorVisualizationConfig(const cXMLElement* vis) :
    SensorVisualizationConfig()
{
    if (vis) {
        auto attr_enabler = [vis](const char* name, bool& field) {
            cXMLElement* elem = vis->getFirstChildWithTag(name);
            if (elem) {
                const char* elem_value = elem->getAttribute("value");
                field = std::strcmp(elem_value, "true") == 0;
            }
        };

        attr_enabler("showObjectsInSensorRange", objectsInSensorRange);
        attr_enabler("showObstaclesInSensorRange", obstaclesInSensorRange);
        attr_enabler("showSensorCone", sensorCone);
        attr_enabler("showLineOfSight",linesOfSight);
    }
}

} // namespace artery
