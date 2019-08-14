/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2018 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_STATIONARYMIDDLEWARE_H_IE9M1YJ3
#define ARTERY_STATIONARYMIDDLEWARE_H_IE9M1YJ3

#include "artery/application/Middleware.h"
#include "artery/application/LocalDynamicMap.h"
#include "artery/application/VehicleDataProvider.h"

namespace artery
{

class StationaryMiddleware : public Middleware
{
    public:
        StationaryMiddleware();
        void initialize(int stage) override;

    protected:
        void handleMessage(omnetpp::cMessage* msg) override;

    private:
        void updateServices();

    private:
        LocalDynamicMap mLocalDynamicMap;
        VehicleDataProvider mVehicleDataProvider;
};

} // namespace artery

#endif /* ARTERY_STATIONARYMIDDLEWARE_H_IE9M1YJ3 */

