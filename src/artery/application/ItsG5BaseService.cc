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

#include "artery/application/ItsG5Service.h"
#include "artery/application/ItsG5Middleware.h"
#include "veins/base/utils/FindModule.h"
#include <cassert>

ItsG5BaseService::ItsG5BaseService() :
		m_facilities(nullptr), m_middleware(nullptr)
{
}

ItsG5BaseService::~ItsG5BaseService()
{
}

Facilities& ItsG5BaseService::getFacilities()
{
	assert(m_facilities);
	return *m_facilities;
}

bool ItsG5BaseService::requiresListener() const
{
	return true;
}

ItsG5BaseService::port_type ItsG5BaseService::getPortNumber() const
{
	assert(m_middleware);
	return m_middleware->getPortNumber(this);
}

cModule* ItsG5BaseService::findHost()
{
	return FindModule<>::findHost(this);
}

void ItsG5BaseService::initialize()
{
	ItsG5Middleware* middleware = dynamic_cast<ItsG5Middleware*>(getParentModule());
	if (middleware == nullptr) {
		opp_error("Middleware not found");
	}

	m_facilities = middleware->getFacilities();
	m_middleware = middleware;
}

void ItsG5BaseService::subscribe(const simsignal_t& signal)
{
	assert(m_middleware);
	m_middleware->subscribe(signal, this);
}

void ItsG5BaseService::unsubscribe(const simsignal_t& signal)
{
	assert(m_middleware);
	m_middleware->unsubscribe(signal, this);
}

void ItsG5BaseService::trigger()
{
}

void ItsG5BaseService::request(const vanetza::btp::DataRequestB& req, std::unique_ptr<DownPacket> packet)
{
	assert(m_middleware);
	m_middleware->request(req, std::move(packet));
}

void ItsG5BaseService::indicate(const vanetza::btp::DataIndication& ind, std::unique_ptr<UpPacket> packet)
{
}
