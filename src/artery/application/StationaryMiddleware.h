/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_STATIONARYMIDDLEWARE_H_IE9M1YJ3
#define ARTERY_STATIONARYMIDDLEWARE_H_IE9M1YJ3

#include "artery/application/Middleware.h"
#include "traci/Listener.h"

namespace artery
{

class StationaryMiddleware : public Middleware, public traci::Listener
{
	public:
		void initialize(int stage) override;

	protected:
		void initializeManagementInformationBase(vanetza::geonet::MIB&) override;
		void traciInit() override;

	private:
		void initializePosition();
};

} // namespace artery

#endif /* ARTERY_STATIONARYMIDDLEWARE_H_IE9M1YJ3 */

