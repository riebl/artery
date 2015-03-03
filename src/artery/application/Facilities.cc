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

#include "Facilities.h"

Facilities::Facilities(
		const VehicleDataProvider& vdp,
		Veins::TraCIMobility& mob,
		const vanetza::dcc::StateMachine& fsm,
		vanetza::dcc::Scheduler& scheduler
) :
		m_vdp(vdp),
		m_mobility(mob),
		m_dcc_fsm(fsm),
		m_dcc_scheduler(scheduler)
{
}

