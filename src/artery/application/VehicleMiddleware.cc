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
#include <vanetza/common/position_fix.hpp>

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
	mGnStationType = vanetza::geonet::StationType::PASSENGER_CAR;
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
	using namespace vanetza::units;
	static const TrueNorth north;
	vanetza::PositionFix position_fix;
	position_fix.timestamp = getRuntime().now();
	position_fix.latitude = mVehicleDataProvider.latitude();
	position_fix.longitude = mVehicleDataProvider.longitude();
	position_fix.confidence.semi_minor = 5.0 * si::meter;
	position_fix.confidence.semi_major = 5.0 * si::meter;
	position_fix.course.assign(north + GeoAngle { mVehicleDataProvider.heading() }, north + 3.0 * degree);
	position_fix.speed.assign(mVehicleDataProvider.speed(), 1.0 * si::meter_per_second);
	mPositionProvider.position_fix(position_fix);
	getRouter().update_position(position_fix);
}

} // namespace artery

