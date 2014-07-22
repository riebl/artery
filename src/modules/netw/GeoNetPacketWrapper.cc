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

#include "GeoNetPacketWrapper.h"
#include <cassert>

using vanetza::geonet::DownPacket;
using vanetza::geonet::UpPacket;

GeoNetPacketWrapper::GeoNetPacketWrapper()
{
}

GeoNetPacketWrapper::GeoNetPacketWrapper(std::unique_ptr<DownPacket> packet) :
		mPacket(new UpPacket())
{
	*mPacket = std::move(*packet);
}

GeoNetPacketWrapper::GeoNetPacketWrapper(std::unique_ptr<UpPacket> packet) :
		mPacket(std::move(packet))
{
}

GeoNetPacketWrapper::~GeoNetPacketWrapper()
{
}

GeoNetPacketWrapper::GeoNetPacketWrapper(const GeoNetPacketWrapper& other)
{
	*this = other;
}

GeoNetPacketWrapper& GeoNetPacketWrapper::operator=(const GeoNetPacketWrapper& other)
{
	if (other.mPacket) {
		mPacket.reset(new UpPacket(*other.mPacket));
	} else {
		mPacket.reset();
	}
	return *this;
}

std::unique_ptr<vanetza::geonet::UpPacket> GeoNetPacketWrapper::extract_up_packet()
{
	return std::move(mPacket);
}
