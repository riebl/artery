/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2020 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_STATIONARYMIDDLEWARE_H_IE9M1YJ3
#define ARTERY_STATIONARYMIDDLEWARE_H_IE9M1YJ3

#include "artery/application/Middleware.h"
#include "artery/utility/Geometry.h"

namespace artery
{

class StationaryMiddleware : public Middleware
{
    public:
        void initialize(int stage) override;
        void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, omnetpp::cObject*, omnetpp::cObject*) override;

    private:
        GeoPosition mGeoPosition;
};

} // namespace artery

#endif /* ARTERY_STATIONARYMIDDLEWARE_H_IE9M1YJ3 */

