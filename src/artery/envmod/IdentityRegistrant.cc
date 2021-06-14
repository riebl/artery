#include "artery/envmod/IdentityRegistrant.h"
#include "artery/utility/IdentityRegistry.h"
#include "artery/traci/VehicleMobility.h"
#include "inet/common/ModuleAccess.h"

namespace artery
{

Define_Module(IdentityRegistrant)

void IdentityRegistrant::initialize()
{
	initializeIdentity();
	registerIdentity();
}

void IdentityRegistrant::initializeIdentity()
{
	auto parent = this->getParentModule();
	auto mobility = inet::getModuleFromPar<MobilityBase>(par("mobilityModule"), this);

	mIdentity.host = parent;
	mIdentity.application = Identity::deriveStationId(parent, par("stationIdDerivation").stringValue());
	mIdentity.traci = mobility->getId();
}

void IdentityRegistrant::registerIdentity()
{
	emit(artery::IdentityRegistry::updateSignal, &mIdentity);
}

void IdentityRegistrant::finish()
{
	emit(artery::IdentityRegistry::removeSignal, &mIdentity);
}

} // namespace artery
