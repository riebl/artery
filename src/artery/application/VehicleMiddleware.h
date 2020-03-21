/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2018 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_VEHICLEMIDDLEWARE_H_SYJDG2DX
#define ARTERY_VEHICLEMIDDLEWARE_H_SYJDG2DX

#include "artery/application/Middleware.h"
#include "artery/application/VehicleDataProvider.h"
#include "artery/traci/VehicleController.h"

namespace artery
{

class VehicleMiddleware : public Middleware
{
    public:
        void initialize(int stage) override;
        void finish() override;

    protected:
        void initializeStationType(const std::string&);
        void initializeVehicleController(omnetpp::cPar&);
        void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, omnetpp::cObject*, omnetpp::cObject*) override;

    private:
        traci::VehicleController* mVehicleController = nullptr;
        VehicleDataProvider mVehicleDataProvider;
};

} // namespace artery

#endif /* ARTERY_VEHICLEMIDDLEWARE_H_SYJDG2DX */

