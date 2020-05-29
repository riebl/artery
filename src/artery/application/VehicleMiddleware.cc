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

using namespace omnetpp;

namespace artery
{

Define_Module(VehicleMiddleware)

VehicleMiddleware::VehicleMiddleware() :
    mVehicleDataProvider(Identity::randomStationId(getRNG(0)))
{
}

void VehicleMiddleware::initialize(int stage)
{
    if (stage == InitStages::Self) {
        findHost()->subscribe(MobilityBase::stateChangedSignal, this);
        initializeVehicleController(par("mobilityModule"));
        initializeStationType(mVehicleController->getVehicleClass());
        getFacilities().register_const(&mVehicleDataProvider);
        mVehicleDataProvider.update(getKinematics(*mVehicleController));

        Identity identity;
        identity.traci = mVehicleController->getVehicleId();
        identity.application = mVehicleDataProvider.station_id();
        emit(Identity::changeSignal, Identity::ChangeTraCI | Identity::ChangeStationId, &identity);
    }

    Middleware::initialize(stage);
}

void VehicleMiddleware::finish()
{
    Middleware::finish();
    findHost()->unsubscribe(MobilityBase::stateChangedSignal, this);
}

void VehicleMiddleware::initializeStationType(const std::string& vclass)
{
    using vanetza::geonet::StationType;
    StationType gnStationType;
    if (vclass == "passenger" || vclass == "private" || vclass == "taxi") {
        gnStationType = StationType::Passenger_Car;
    } else if (vclass == "coach" || vclass == "delivery") {
        gnStationType = StationType::Light_Truck;
    } else if (vclass == "truck") {
        gnStationType = StationType::Heavy_Truck;
    } else if (vclass == "trailer") {
        gnStationType = StationType::Trailer;
    } else if (vclass == "bus") {
        gnStationType = StationType::Bus;
    } else if (vclass == "emergency" || vclass == "authority") {
        gnStationType = StationType::Special_Vehicle;
    } else if (vclass == "moped") {
        gnStationType = StationType::Moped;
    } else if (vclass == "motorcycle") {
        gnStationType = StationType::Motorcycle;
    } else if (vclass == "tram") {
        gnStationType = StationType::Tram;
    } else if (vclass == "bicycle") {
        gnStationType = StationType::Cyclist;
    } else if (vclass == "pedestrian") {
        gnStationType = StationType::Pedestrian;
    } else {
        gnStationType = StationType::Unknown;
    }

    setStationType(gnStationType);
    mVehicleDataProvider.setStationType(gnStationType);
}

void VehicleMiddleware::initializeVehicleController(cPar& mobilityPar)
{
	auto mobility = inet::getModuleFromPar<ControllableVehicle>(mobilityPar, findHost());
	mVehicleController = mobility->getVehicleController();
	ASSERT(mVehicleController);
	getFacilities().register_mutable(mVehicleController);
}

void VehicleMiddleware::receiveSignal(cComponent* component, simsignal_t signal, cObject* obj, cObject* details)
{
	if (signal == MobilityBase::stateChangedSignal && mVehicleController) {
		mVehicleDataProvider.update(getKinematics(*mVehicleController));
	}
}

} // namespace artery

