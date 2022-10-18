/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/StationType.h"
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
    mVehicleDataProvider(0) // OMNeT++ assigns RNG after construction: set final station ID later
{
}

void VehicleMiddleware::initialize(int stage)
{
    if (stage == InitStages::Self) {
        findHost()->subscribe(MobilityBase::stateChangedSignal, this);
        initializeVehicleController(par("mobilityModule"));
        initializeStationType(mVehicleController->getVehicleClass());

        Identity identity;
        identity.traci = mVehicleController->getVehicleId();
        identity.application = Identity::deriveStationId(findHost(), par("stationIdDerivation").stringValue());
        emit(Identity::changeSignal, Identity::ChangeTraCI | Identity::ChangeStationId, &identity);

        mVehicleDataProvider.setStationId(identity.application);
        mVehicleDataProvider.update(getKinematics(*mVehicleController));
        getFacilities().register_const(&mVehicleDataProvider);
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
    auto gnStationType = deriveStationTypeFromVehicleClass(vclass);
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

