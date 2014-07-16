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

#include <cmessage.h>
#include <simutil.h>
#include <vanetza/common/byte_buffer_convertible.hpp>

namespace vanetza {
namespace convertible {

template<>
struct byte_buffer_impl<cPacket&> : public byte_buffer
{
	byte_buffer_impl(cPacket& packet) : m_packet(packet) {}
	void convert(ByteBuffer& buf) const override { opp_error("Can't serialize cPacket"); }
	std::size_t size() const override { return m_packet.getByteLength(); }
	std::unique_ptr<byte_buffer> duplicate() const override
	{
		return std::unique_ptr<byte_buffer> {
			new byte_buffer_impl<cPacket&>(*m_packet.dup())
		};
	}

	cPacket& m_packet;
};

}
}

#endif /* CPACKET_BYTE_BUFFER_CONVERTIBLE_H_ */
