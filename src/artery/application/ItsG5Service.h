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

#ifndef ARTERY_ITSG5SERVICE_H_
#define ARTERY_ITSG5SERVICE_H_

#include "ItsG5BaseService.h"

namespace artery
{

class ItsG5Service : public ItsG5BaseService
{
	public:
		ItsG5Service();
		virtual ~ItsG5Service();

	protected:
		using ItsG5BaseService::indicate;
		using ItsG5BaseService::request;
		using ItsG5BaseService::initialize;
		void indicate(const vanetza::btp::DataIndication&, std::unique_ptr<vanetza::UpPacket>, const NetworkInterface& interface) override;
		virtual void indicate(const vanetza::btp::DataIndication&, omnetpp::cPacket*);
		virtual void indicate(const vanetza::btp::DataIndication&, omnetpp::cPacket*, const NetworkInterface& interface);
		void request(const vanetza::btp::DataRequestB&, omnetpp::cPacket*, const NetworkInterface* = nullptr);
};

} // namespace artery

#endif /* ARTERY_ITSG5SERVICE_H_ */
