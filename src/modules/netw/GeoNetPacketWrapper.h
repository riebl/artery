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

#ifndef GEONETPACKETWRAPPER_H_
#define GEONETPACKETWRAPPER_H_

#include <vanetza/geonet/packet.hpp>
#include <memory>

class GeoNetPacketWrapper
{
	public:
		GeoNetPacketWrapper();
		GeoNetPacketWrapper(std::unique_ptr<vanetza::geonet::DownPacket>);
		GeoNetPacketWrapper(std::unique_ptr<vanetza::geonet::UpPacket>);
		GeoNetPacketWrapper(const GeoNetPacketWrapper&);
		GeoNetPacketWrapper& operator=(const GeoNetPacketWrapper&);
		virtual ~GeoNetPacketWrapper();
		std::unique_ptr<vanetza::geonet::UpPacket> extract_up_packet();

	private:
		std::unique_ptr<vanetza::geonet::UpPacket> mPacket;
};

#endif /* GEONETPACKETWRAPPER_H_ */
