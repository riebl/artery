/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2018 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_VEHICLEMIDDLEWARE_H_SYJDG2DX
#define ARTERY_VEHICLEMIDDLEWARE_H_SYJDG2DX

#include "artery/application/Middleware.h"
#include "artery/application/VehicleDataProvider.h"
#include <omnetpp/clistener.h>

namespace artery
{

class VehicleMiddleware : public Middleware, public omnetpp::cListener
{
    public:
        void initialize(int stage) override;
        void finish() override;

    protected:
        void initializeIdentity(Identity&) override;
        void initializeStationType(const std::string&);
        void initializeVehicleController(omnetpp::cPar&);
        void receiveSignal(cComponent*, omnetpp::simsignal_t, cObject*, cObject*) override;

    private:
        traci::VehicleController* mVehicleController = nullptr;
        VehicleDataProvider mVehicleDataProvider;
};

} // namespace artery

#endif /* ARTERY_VEHICLEMIDDLEWARE_H_SYJDG2DX */

