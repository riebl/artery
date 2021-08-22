/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ENVMOD_BASESENSOR_H_
#define ENVMOD_BASESENSOR_H_

#include "artery/envmod/sensor/Sensor.h"

namespace artery
{

// forward declarations
class Facilities;
class GlobalEnvironmentModel;
class LocalEnvironmentModel;
class Middleware;

class BaseSensor : public Sensor
{
public:
    BaseSensor();

protected:
    void initialize() override;
    Facilities& getFacilities();
    const Facilities& getFacilities() const;
    Middleware& getMiddleware();
    const Middleware& getMiddleware() const;
    std::string getEgoId();
    cModule* findHost();

    LocalEnvironmentModel* mLocalEnvironmentModel;
    GlobalEnvironmentModel* mGlobalEnvironmentModel;

private:
    Middleware* mMiddleware;
};

} // namespace artery

#endif /* ENVMOD_BASESENSOR_H_ */
