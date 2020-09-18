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

#ifndef CPACKET_BYTE_BUFFER_CONVERTIBLE_H_
#define CPACKET_BYTE_BUFFER_CONVERTIBLE_H_

#include <omnetpp/cobject.h>
#include <omnetpp/cpacket.h>
#include <vanetza/common/byte_buffer_convertible.hpp>
#include <cassert>
#include <memory>

namespace vanetza {
namespace convertible {

template<>
class byte_buffer_impl<omnetpp::cPacket*> : public byte_buffer, private omnetpp::cObject
{
	public:
		byte_buffer_impl(omnetpp::cPacket* packet) : m_packet(packet)
		{
			assert(packet);
			take(m_packet.get());
		}

		~byte_buffer_impl()
		{
			drop_packet();
		}

		void convert(ByteBuffer& buf) const override
		{
			// Dummy serialization for faking cryptographic signatures of cPackets
			EV_DETAIL << "cPacket serialization is only a dummy implementation";
			buf.resize(size());
		}

		std::size_t size() const override
		{
			return m_packet->getByteLength();
		}

		std::unique_ptr<byte_buffer> duplicate() const override
		{
			return std::unique_ptr<byte_buffer> {
				new byte_buffer_impl<omnetpp::cPacket*>(m_packet->dup())
			};
		}

		omnetpp::cPacket* consume()
		{
			drop_packet();
			return m_packet.release();
		}

	private:
		void drop_packet()
		{
			if (m_packet) {
				drop(m_packet.get());
			}
		}

		std::unique_ptr<omnetpp::cPacket> m_packet;
};

}
}

#endif /* CPACKET_BYTE_BUFFER_CONVERTIBLE_H_ */
