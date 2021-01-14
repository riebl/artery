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
#include "artery/application/Middleware.h"
#include <inet/common/ModuleAccess.h>
#include <omnetpp/clog.h>
#include <cassert>

using namespace omnetpp;

namespace artery
{

ItsG5BaseService::ItsG5BaseService() :
	m_middleware(nullptr)
{
}

ItsG5BaseService::~ItsG5BaseService()
{
}

Facilities& ItsG5BaseService::getFacilities()
{
	assert(m_middleware);
	return m_middleware->getFacilities();
}

const Facilities& ItsG5BaseService::getFacilities() const
{
	ASSERT(m_middleware);
	return m_middleware->getFacilities();
}

bool ItsG5BaseService::requiresListener() const
{
	return true;
}

void ItsG5BaseService::addTransportDescriptor(const TransportDescriptor& td)
{
	m_listeners.insert(td);
}

PortNumber ItsG5BaseService::getPortNumber(ChannelNumber ch) const
{
	for (const auto& listener : m_listeners) {
		if (getChannel(listener) == ch) {
			return getPort(listener);
		}
	}

	EV_WARN << "No listening port specified on channel " << ch << "\n";
	return 0;
}

cModule* ItsG5BaseService::findHost()
{
	return inet::findContainingNode(m_middleware);
}

void ItsG5BaseService::initialize()
{
	auto* parent = getParentModule();
	// if this service is part of a compound module, the Middleware is its grand parent
	if (!parent->getModuleType()->isSimple()) {
		parent = parent->getParentModule();
	}
	Middleware* middleware = dynamic_cast<Middleware*>(parent);
	if (middleware == nullptr) {
		throw cRuntimeError("Middleware not found");
	}

	m_middleware = middleware;
}

void ItsG5BaseService::finish()
{
	cSimpleModule::finish();
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

void ItsG5BaseService::request(const vanetza::btp::DataRequestB& req,
	std::unique_ptr<vanetza::DownPacket> packet, const NetworkInterface* interface)
{
	assert(m_middleware);
	if (interface) {
		m_middleware->requestTransmission(req, std::move(packet), *interface);
	} else {
		m_middleware->requestTransmission(req, std::move(packet));
	}
}

void ItsG5BaseService::indicate(const vanetza::btp::DataIndication& ind,
	std::unique_ptr<vanetza::UpPacket> packet, const NetworkInterface&)
{
	// forward indication to "old" indicate method by default
	this->indicate(ind, std::move(packet));
}

void ItsG5BaseService::indicate(const vanetza::btp::DataIndication& ind, std::unique_ptr<vanetza::UpPacket> packet)
{
	// no-op by default
}

} // namespace artery
