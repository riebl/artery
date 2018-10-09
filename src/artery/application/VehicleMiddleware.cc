/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/VehicleMiddleware.h"
#include "artery/traci/ControllableVehicle.h"
#include "artery/traci/MobilityBase.h"
#include "artery/utility/InitStages.h"
#include "inet/common/ModuleAccess.h"

namespace artery
{

Define_Module(VehicleMiddleware)

void VehicleMiddleware::initialize(int stage)
{
    if (stage == InitStages::Self) {
        findHost()->subscribe(MobilityBase::stateChangedSignal, this);
        initializeVehicleController(par("mobilityModule"));
        initializeStationType(mVehicleController->getVehicleClass());
        getFacilities().register_const(&mVehicleDataProvider);
        mVehicleDataProvider.update(mVehicleController);
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

void VehicleMiddleware::initializeStationType(const std::string& vclass)
{
    using vanetza::geonet::StationType;
    StationType gnStationType;
    if (vclass == "passenger" || vclass == "private" || vclass == "taxi") {
        gnStationType = StationType::PASSENGER_CAR;
    } else if (vclass == "coach" || vclass == "delivery") {
        gnStationType = StationType::LIGHT_TRUCK;
    } else if (vclass == "truck") {
        gnStationType = StationType::HEAVY_TRUCK;
    } else if (vclass == "trailer") {
        gnStationType = StationType::TRAILER;
    } else if (vclass == "bus") {
        gnStationType = StationType::BUS;
    } else if (vclass == "emergency" || vclass == "authority") {
        gnStationType = StationType::SPECIAL_VEHICLE;
    } else if (vclass == "moped") {
        gnStationType = StationType::MOPED;
    } else if (vclass == "motorcycle") {
        gnStationType = StationType::MOTORCYCLE;
    } else if (vclass == "tram") {
        gnStationType = StationType::TRAM;
    } else if (vclass == "bicycle") {
        gnStationType = StationType::CYCLIST;
    } else if (vclass == "pedestrian") {
        gnStationType = StationType::PEDESTRIAN;
    } else {
        gnStationType = StationType::UNKNOWN;
    }

    setStationType(gnStationType);
    mVehicleDataProvider.setStationType(gnStationType);
}

void VehicleMiddleware::initializeVehicleController(omnetpp::cPar& mobilityPar)
{
	auto mobility = inet::getModuleFromPar<ControllableVehicle>(mobilityPar, findHost());
	mVehicleController = mobility->getVehicleController();
	ASSERT(mVehicleController);
	getFacilities().register_mutable(mVehicleController);
}

void VehicleMiddleware::receiveSignal(cComponent* component, simsignal_t signal, cObject* obj, cObject* details)
{
	if (signal == MobilityBase::stateChangedSignal) {
		mVehicleDataProvider.update(mVehicleController);
	}
}

} // namespace artery

