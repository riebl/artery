/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ENVMOD_BASESENSOR_H_
#define ENVMOD_BASESENSOR_H_

#include "artery/envmod/sensor/Sensor.h"
#include "artery/envmod/sensor/SensorVisualizationConfig.h"

// forward declarations
class Facilities;

namespace artery
{

// forward declarations
class GlobalEnvironmentModel;
class LocalEnvironmentModel;
class Middleware;

class BaseSensor : public Sensor
{
public:
    BaseSensor();
    void setVisualization(const SensorVisualizationConfig& cfg) override { mVisualizationConfig = cfg; }

protected:
    void initialize() override;
    Facilities& getFacilities();
    Middleware& getMiddleware();
    std::string getEgoId();
    cModule* findHost();

    LocalEnvironmentModel* mLocalEnvironmentModel;
    GlobalEnvironmentModel* mGlobalEnvironmentModel;
    SensorVisualizationConfig mVisualizationConfig;

private:
    Middleware* mMiddleware;
};

} // namespace artery

#endif /* ENVMOD_BASESENSOR_H_ */
