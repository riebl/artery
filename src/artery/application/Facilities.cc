//
// Copyright (C) 2014 Raphael Riebl <raphael.riebl@thi.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "artery/application/Facilities.h"
#include "artery/application/VehicleDataProvider.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"
#include <vanetza/dcc/scheduler.hpp>
#include <vanetza/dcc/state_machine.hpp>

const VehicleDataProvider& Facilities::getVehicleDataProvider() const
{
	return get_const<VehicleDataProvider>();
}

vanetza::dcc::Scheduler& Facilities::getDccScheduler()
{
	return get_mutable<vanetza::dcc::Scheduler>();
}

const vanetza::dcc::StateMachine& Facilities::getDccStateMachine() const
{
	return get_const<vanetza::dcc::StateMachine>();
}
