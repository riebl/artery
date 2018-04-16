/*
 * Artery V2X Simulation Framework
 * Copyright 2017-2018 Christina Obermaier
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_OTA_INTERFACE_H
#define ARTERY_OTA_INTERFACE_H

#include "artery/messages/GeoNetPacket_m.h"
#include <vanetza/common/byte_view.hpp>
#include <vanetza/net/mac_address.hpp>
#include <memory>

namespace artery
{

class OtaInterfaceLayer;

class OtaInterface
{
public:
    virtual void registerModule(OtaInterfaceLayer*) = 0;
    virtual void unregisterModule() = 0;
    virtual void sendMessage(const vanetza::MacAddress& source, const vanetza::MacAddress& destination, const vanetza::byte_view_range& data) = 0;
    virtual void receiveMessage(std::unique_ptr<GeoNetPacket>) = 0;
    virtual bool hasRegisteredModule() = 0;

    virtual ~OtaInterface() = default;
};

} // namespace artery

#endif /* ARTERY_OTA_INTERFACE_H */
