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

#ifndef ITSG5BASESERVICE_H_
#define ITSG5BASESERVICE_H_

#include <clistener.h>
#include <csimplemodule.h>
#include <vanetza/btp/data_interface.hpp>
#include <vanetza/btp/data_request.hpp>
#include "Facilities.h"
#include "ItsG5Middleware.h"

class ItsG5BaseService :
	public cSimpleModule, public cListener,
	public vanetza::btp::IndicationInterface
{
	public:
		typedef ItsG5Middleware::port_type port_type;

		ItsG5BaseService();
		virtual ~ItsG5BaseService();
		virtual void trigger();
		virtual bool requiresListener() const;

	protected:
		void initialize() override;
		void request(const vanetza::btp::DataRequestB&, std::unique_ptr<vanetza::btp::DownPacket>);
		void indicate(const vanetza::btp::DataIndication&, std::unique_ptr<vanetza::btp::UpPacket>) override;
		Facilities& getFacilities();
		port_type getPortNumber() const;
		cModule* findHost();
		void subscribe(const simsignal_t&);
		void unsubscribe(const simsignal_t&);

	private:
		Facilities* m_facilities;
		ItsG5Middleware* m_middleware;
};

#endif /* ITSG5BASESERVICE_H_ */
