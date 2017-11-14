/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/StationaryMiddleware.h"
#include "traci/Core.h"
#include "traci/LiteAPI.h"
#include "traci/Position.h"
#include <boost/units/systems/si/plane_angle.hpp>
#include <inet/common/ModuleAccess.h>
#include <inet/mobility/contract/IMobility.h>
#include <cmath>

namespace artery
{

Define_Module(StationaryMiddleware)

void StationaryMiddleware::initialize(int stage)
{
	Middleware::initialize(stage);

	if (stage == 0) {
		Listener::subscribeTraCI(getSystemModule());
	}
}

void StationaryMiddleware::initializeManagementInformationBase(vanetza::geonet::MIB& mib)
{
	Middleware::initializeManagementInformationBase(mib);
	mib.itsGnStationType = vanetza::geonet::StationType::RSU;
	mib.itsGnIsMobile = false;
}

void StationaryMiddleware::traciInit()
{
	initializePosition();
}

void StationaryMiddleware::initializePosition()
{
	auto host = findHost();
	auto mobility = inet::getModuleFromPar<inet::IMobility>(par("mobilityModule"), host);
	inet::Coord pos = mobility->getCurrentPosition();

	// TODO inet::IGeographicCoordinateSystem provided by TraCI module would be nice
	auto traci = inet::getModuleFromPar<traci::Core>(par("traciCoreModule"), host);
	traci::LiteAPI& api = traci->getLiteAPI();
	const traci::TraCIBoundary& boundary = api.simulation().getNetBoundary();
	traci::TraCIGeoPosition geopos = api.convertGeo(traci::position_cast(boundary, Position { pos.x, pos.y }));

	using boost::units::degree::degree;
	vanetza::geonet::LongPositionVector lpv;
	lpv.timestamp = vanetza::geonet::Timestamp(getRuntime().now());
	lpv.latitude = static_cast<decltype(lpv.latitude)>(geopos.latitude * degree);
	lpv.longitude = static_cast<decltype(lpv.longitude)>(geopos.longitude * degree);
	lpv.position_accuracy_indicator = true;
	getRouter().update(lpv);
}

} // namespace artery
