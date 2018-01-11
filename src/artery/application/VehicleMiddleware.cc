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
	using vanetza::geonet::StationType;
	Middleware::initializeManagementInformationBase(mib);
	assert(mVehicleController);
	const std::string vclass = mVehicleController->getVehicleClass();
	if (vclass == "passenger" || vclass == "private" || vclass == "taxi") {
		mGnStationType = StationType::PASSENGER_CAR;
	} else if (vclass == "coach" || vclass == "delivery") {
		mGnStationType = StationType::LIGHT_TRUCK;
	} else if (vclass == "truck") {
		mGnStationType = StationType::HEAVY_TRUCK;
	} else if (vclass == "trailer") {
		mGnStationType = StationType::TRAILER;
	} else if (vclass == "bus") {
		mGnStationType = StationType::BUS;
	} else if (vclass == "emergency" || vclass == "authority") {
		mGnStationType = StationType::SPECIAL_VEHICLE;
	} else if (vclass == "moped") {
		mGnStationType = StationType::MOPED;
	} else if (vclass == "motorcycle") {
		mGnStationType = StationType::MOTORCYCLE;
	} else if (vclass == "tram") {
		mGnStationType = StationType::TRAM;
	} else if (vclass == "bicycle") {
		mGnStationType = StationType::CYCLIST;
	} else if (vclass == "pedestrian") {
		mGnStationType = StationType::PEDESTRIAN;
	} else {
		mGnStationType = StationType::UNKNOWN;
	}
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

