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

#ifndef FACILITIES_H_
#define FACILITIES_H_

#include "artery/application/VehicleDataProvider.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"
#include <vanetza/dcc/scheduler.hpp>
#include <vanetza/dcc/state_machine.hpp>

/**
 * Context class for each ITS-G5 service provided by middleware
 */
class Facilities
{
	public:
		Facilities(const VehicleDataProvider&, Veins::TraCIMobility&,
				const vanetza::dcc::StateMachine&, vanetza::dcc::Scheduler&);
		const VehicleDataProvider& getVehicleDataProvider() const { return m_vdp; }
		Veins::TraCIMobility& getMobility() { return m_mobility; }
		vanetza::dcc::Scheduler& getDccScheduler() { return m_dcc_scheduler; }
		const vanetza::dcc::StateMachine& getDccStateMachine() { return m_dcc_fsm; }

	private:
		const VehicleDataProvider& m_vdp;
		Veins::TraCIMobility& m_mobility;
		const vanetza::dcc::StateMachine& m_dcc_fsm;
		vanetza::dcc::Scheduler& m_dcc_scheduler;
};

#endif /* FACILITIES_H_ */
