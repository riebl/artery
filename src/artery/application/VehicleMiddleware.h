/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_VEHICLEMIDDLEWARE_H_SYJDG2DX
#define ARTERY_VEHICLEMIDDLEWARE_H_SYJDG2DX

#include "artery/application/Middleware.h"
#include "artery/application/VehicleDataProvider.h"

namespace artery
{

class VehicleMiddleware : public Middleware
{
	public:
		void initialize(int stage) override;
		void finish() override;

	protected:
		void initializeIdentity(Identity&) override;
		void initializeManagementInformationBase(vanetza::geonet::MIB&) override;
		void receiveSignal(cComponent*, omnetpp::simsignal_t, cObject*, cObject*) override;
		void update() override;

	private:
		void initializeVehicleController();
		void updatePosition();

		traci::VehicleController* mVehicleController;
		VehicleDataProvider mVehicleDataProvider;
};

} // namespace artery

#endif /* ARTERY_VEHICLEMIDDLEWARE_H_SYJDG2DX */

