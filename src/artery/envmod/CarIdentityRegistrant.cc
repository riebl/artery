#include "artery/envmod/CarIdentityRegistrant.h"
#include "artery/utility/IdentityRegistry.h"
#include "artery/traci/MobilityBase.h"
#include "inet/common/ModuleAccess.h"


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
	auto parent = this->getParentModule();
	auto mobility = dynamic_cast<MobilityBase*>(parent->getSubmodule("mobility", -1));
	if (!mobility) {
		throw omnetpp::cRuntimeError("no suitable mobility module found");
	}
	auto controller = mobility->getVehicleController();

	auto traciId = controller->getVehicleId();

	auto path = parent->getFullPath();
	auto i0 = path.find("[");
	auto i1 = path.find("]");
	auto stationId = std::stoul(path.substr(i0+1, i1-i0-1));

	mIdentity.host = parent;
	mIdentity.application = stationId;
	mIdentity.traci = traciId;
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
