/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2021 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/StationType.h"
#include "artery/application/PersonMiddleware.h"
#include "artery/traci/ControllablePerson.h"
#include "artery/traci/MobilityBase.h"
#include "artery/utility/InitStages.h"
#include "inet/common/ModuleAccess.h"

using namespace omnetpp;

namespace artery
{

Define_Module(PersonMiddleware)

PersonMiddleware::PersonMiddleware() :
    mVehicleDataProvider(0) // OMNeT++ assigns RNG after construction: set final station ID later
{
}

void PersonMiddleware::initialize(int stage)
{
    if (stage == InitStages::Self) {
        findHost()->subscribe(MobilityBase::stateChangedSignal, this);
        initializePersonController(par("mobilityModule"));
        initializeStationType();

        Identity identity;
        identity.traci = mPersonController->getPersonId();
        identity.application = Identity::deriveStationId(findHost(), par("stationIdDerivation").stringValue());
        emit(Identity::changeSignal, Identity::ChangeTraCI | Identity::ChangeStationId, &identity);

        mVehicleDataProvider.setStationId(identity.application);
        mVehicleDataProvider.update(getKinematics(*mPersonController));
        getFacilities().register_const(&mVehicleDataProvider);
    }

    Middleware::initialize(stage);
}

void PersonMiddleware::finish()
{
    Middleware::finish();
    findHost()->unsubscribe(MobilityBase::stateChangedSignal, this);
}

void PersonMiddleware::initializeStationType()
{
    auto gnStationType = vanetza::geonet::StationType::Pedestrian;
    setStationType(gnStationType);
    mVehicleDataProvider.setStationType(gnStationType);
}

void PersonMiddleware::initializePersonController(cPar& mobilityPar)
{
	auto mobility = inet::getModuleFromPar<ControllablePerson>(mobilityPar, findHost());
	mPersonController = mobility->getPersonController();
	ASSERT(mPersonController);
	getFacilities().register_mutable(mPersonController);
}

void PersonMiddleware::receiveSignal(cComponent* component, simsignal_t signal, cObject* obj, cObject* details)
{
	if (signal == MobilityBase::stateChangedSignal && mPersonController) {
		mVehicleDataProvider.update(getKinematics(*mPersonController));
	}
}

} // namespace artery

