/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/StationaryMiddleware.h"
#include "traci/Core.h"
#include "traci/LiteAPI.h"
#include "traci/Position.h"
#include <inet/common/ModuleAccess.h>
#include <inet/mobility/contract/IMobility.h>
#include <vanetza/common/position_fix.hpp>
#include <veins/base/modules/BaseMobility.h>
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
	mGnStationType = vanetza::geonet::StationType::RSU;
	mib.itsGnIsMobile = false;
}

void StationaryMiddleware::traciInit()
{
	initializePosition();
}

void StationaryMiddleware::initializePosition()
{
	Position pos;
	auto host = findHost();
	auto mobilityModule = host->getModuleByPath(par("mobilityModule").stringValue());
	if (auto mobility = dynamic_cast<inet::IMobility*>(mobilityModule)) {
		inet::Coord inet_pos = mobility->getCurrentPosition();
		pos = Position { inet_pos.x, inet_pos.y };
	} else if (auto mobility = dynamic_cast<BaseMobility*>(mobilityModule)) {
		Coord veins_pos = mobility->getCurrentPosition();
		pos = Position { veins_pos.x, veins_pos.y };
	} else {
		throw cRuntimeError("no suitable mobility module found");
	}

	// TODO inet::IGeographicCoordinateSystem provided by TraCI module would be nice
	auto traci = inet::getModuleFromPar<traci::Core>(par("traciCoreModule"), host);
	traci::LiteAPI& api = traci->getLiteAPI();
	const traci::TraCIBoundary& boundary = api.simulation().getNetBoundary();
	traci::TraCIGeoPosition geopos = api.convertGeo(traci::position_cast(boundary, Position { pos.x, pos.y }));

	using namespace vanetza::units;
	vanetza::PositionFix position_fix;
	position_fix.timestamp = getRuntime().now();
	position_fix.latitude = geopos.latitude * degree;
	position_fix.longitude = geopos.longitude * degree;
	position_fix.confidence.semi_minor = 1.0 * si::meter;
	position_fix.confidence.semi_major = 1.0 * si::meter;
	position_fix.speed.assign(0.0 * si::meter_per_second, 0.0 * si::meter_per_second);
	mPositionProvider.position_fix(position_fix);
	getRouter().update_position(position_fix);
}

} // namespace artery
