//
// Copyright (C) 2015 Raphael Riebl <raphael.riebl@thi.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef ARTERY_ITSG5PROMISCUOUSSERVICE_H_
#define ARTERY_ITSG5PROMISCUOUSSERVICE_H_

#include "artery/application/ItsG5BaseService.h"
#include "artery/application/PromiscuousHook.h"
#include <vanetza/btp/port_dispatcher.hpp>

namespace artery
{

class ItsG5PromiscuousService :
	public ItsG5BaseService,
	public PromiscuousHook
{
	public:
		void tap_packet(const vanetza::btp::DataIndication&, const vanetza::UpPacket&, NetworkInterface&) override final;

	protected:
		virtual void tapPacket(const vanetza::btp::DataIndication&, const vanetza::UpPacket&, NetworkInterface&) = 0;
};

} // namespace artery

#endif /* ARTERY_ITSG5PROMISCUOUSSERVICE_H_ */
