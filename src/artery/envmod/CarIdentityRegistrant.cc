#include "artery/envmod/CarIdentityRegistrant.h"
#include "artery/utility/IdentityRegistry.h"
#include "artery/traci/MobilityBase.h"


namespace artery
{

Define_Module(CarIdentityRegistrant)

void CarIdentityRegistrant::initialize()
{
	initializeIdentity();
	registerIdentity();
}

void CarIdentityRegistrant::initializeIdentity()
{
	auto parent = getParentModule();
	auto mobility = dynamic_cast<MobilityBase*>(parent->getSubmodule("mobility", -1));
	if (!mobility) {
		throw omnetpp::cRuntimeError("no suitable mobility module found");
	}
	auto controller = mobility->getVehicleController();

	auto traciId = controller->getVehicleId();
	auto componentId = getParentModule()->getId();

	mIdentity.component = componentId;
	mIdentity.application = componentId;
	mIdentity.traci = traciId;
	mIdentity.geonet = vanetza::geonet::Address();
}

void CarIdentityRegistrant::registerIdentity()
{
	emit(artery::IdentityRegistry::updateSignal, &mIdentity);
}

void CarIdentityRegistrant::finish()
{
	emit(artery::IdentityRegistry::removeSignal, &mIdentity);
}

} // namespace artery
