/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/VehicleMiddleware.h"
#include "artery/envmod/LocalEnvironmentModel.h"
#include "artery/envmod/GlobalEnvironmentModel.h"
#include "artery/traci/ControllableVehicle.h"
#include "artery/traci/MobilityBase.h"
#include "inet/common/ModuleAccess.h"

namespace artery
{

Define_Module(VehicleMiddleware)

void VehicleMiddleware::initialize(int stage)
{
	if (stage == 0) {
		findHost()->subscribe(MobilityBase::stateChangedSignal, this);
		getFacilities().register_const(&mVehicleDataProvider);
		initializeVehicleController();
		initializeEnvironmentModel();
	}

	Middleware::initialize(stage);
}

void VehicleMiddleware::finish()
{
	Middleware::finish();
	findHost()->unsubscribe(MobilityBase::stateChangedSignal, this);
}

void VehicleMiddleware::initializeIdentity(Identity& id)
{
	Middleware::initializeIdentity(id);
	id.traci = mVehicleController->getVehicleId();
	id.application = mVehicleDataProvider.station_id();
}

void VehicleMiddleware::initializeManagementInformationBase(vanetza::geonet::MIB& mib)
{
	Middleware::initializeManagementInformationBase(mib);
	// TODO derive station type from SUMO vehicle class
	mib.itsGnStationType = vanetza::geonet::StationType::PASSENGER_CAR;
}

void VehicleMiddleware::initializeVehicleController()
{
	auto mobility = inet::getModuleFromPar<ControllableVehicle>(par("mobilityModule"), findHost());
	mVehicleController = mobility->getVehicleController();
	ASSERT(mVehicleController);
	getFacilities().register_mutable(mVehicleController);
}

void VehicleMiddleware::initializeEnvironmentModel()
{
#ifdef WITH_ENVMOD
	mLocalEnvironmentModel = inet::findModuleFromPar<artery::LocalEnvironmentModel>(par("localEnvironmentModule"), findHost());
	if (mLocalEnvironmentModel) {
		mGlobalEnvironmentModel = inet::getModuleFromPar<artery::GlobalEnvironmentModel>(par("globalEnvironmentModule"), findHost());
		Facilities& fac = getFacilities();
		fac.register_mutable(mLocalEnvironmentModel);
		fac.register_mutable(mGlobalEnvironmentModel);
	}
#endif
}

void VehicleMiddleware::receiveSignal(cComponent* component, simsignal_t signal, cObject* obj, cObject* details)
{
	if (signal == MobilityBase::stateChangedSignal) {
		mVehicleDataProvider.update(mVehicleController);
	}
}

void VehicleMiddleware::update()
{
	updatePosition();
	Middleware::update();
}

void VehicleMiddleware::updatePosition()
{
	vanetza::geonet::LongPositionVector lpv;
	lpv.timestamp = vanetza::geonet::Timestamp(getRuntime().now());
	lpv.latitude = static_cast<decltype(lpv.latitude)>(mVehicleDataProvider.latitude());
	lpv.longitude = static_cast<decltype(lpv.longitude)>(mVehicleDataProvider.longitude());
	lpv.heading = static_cast<decltype(lpv.heading)>(mVehicleDataProvider.heading());
	lpv.speed = static_cast<decltype(lpv.speed)>(mVehicleDataProvider.speed());
	lpv.position_accuracy_indicator = true;
	getRouter().update(lpv);
}

} // namespace artery

