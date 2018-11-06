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

#include "ItsG5Service.h"
#include "cpacket_byte_buffer_convertible.h"
#include <boost/variant/static_visitor.hpp>

using namespace omnetpp;

namespace artery
{

ItsG5Service::ItsG5Service()
{
}

ItsG5Service::~ItsG5Service()
{
}

void ItsG5Service::indicate(const vanetza::btp::DataIndication& ind, std::unique_ptr<vanetza::UpPacket> raw_packet, const NetworkInterface& interface)
{
	using namespace vanetza;

	struct packet_visitor : public boost::static_visitor<cPacket*>
	{
		cPacket* operator()(CohesivePacket& packet)
		{
			throw cRuntimeError("Can't handle CohesivePackets in ItsG5Service");
			return nullptr;
		}

		cPacket* operator()(ChunkPacket& packet)
		{
			cPacket* result = nullptr;
			typedef convertible::byte_buffer byte_buffer;
			typedef convertible::byte_buffer_impl<cPacket*> byte_buffer_impl;

			byte_buffer* ptr = packet[OsiLayer::Application].ptr();
			byte_buffer_impl* impl = dynamic_cast<byte_buffer_impl*>(ptr);
			if (impl != nullptr) {
				result = impl->consume();
			}

			return result;
		}
	};

	packet_visitor visitor;
	cPacket* packet = boost::apply_visitor(visitor, *raw_packet);
	if (packet != nullptr) {
		indicate(ind, packet, interface);
	} else {
		throw cRuntimeError("Unable to extract cPacket out of data indication");
	}
}

void ItsG5Service::indicate(const vanetza::btp::DataIndication& ind, cPacket* packet)
{
	delete packet;
}

void ItsG5Service::indicate(const vanetza::btp::DataIndication& ind, cPacket* packet, const NetworkInterface& interface)
{
	// forward to "old" indicate method by default
	this->indicate(ind, packet);
}

void ItsG5Service::request(const vanetza::btp::DataRequestB& req, cPacket* packet, const NetworkInterface* interface)
{
	std::unique_ptr<vanetza::DownPacket> buffer { new vanetza::DownPacket() };
	buffer->layer(vanetza::OsiLayer::Application) = std::move(packet);
	ItsG5BaseService::request(req, std::move(buffer), interface);
}

} // namespace artery
