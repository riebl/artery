/*
 * Artery V2X Simulation Framework
 * Copyright 2017-2018 Christina Obermaier
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_OTA_INTERFACE_H
#define ARTERY_OTA_INTERFACE_H

#include "artery/networking/GeoNetPacket.h"
#include <vanetza/common/byte_view.hpp>
#include <vanetza/net/mac_address.hpp>
#include <memory>

namespace artery
{

class OtaInterfaceLayer;

/**
 * Interface module for all OtaInterface classes.
 * The OtaInterface is a global OMNeT++ Module which maintains the data exchange between a certain DUT and the interface hardware
 */
class OtaInterface
{
public:
    /**
     * Registers a OtaInterfaceLayer at the OtaInterface
     *
     * \param OtaInterfaceLayer to register
     */
    virtual void registerModule(OtaInterfaceLayer*) = 0;

    /**
     * Unregisters an allready registered OtaInterfaceLayer
     */
    virtual void unregisterModule() = 0;

    /**
     * Transmits a simulated packet to the DUT
     *
     * \param cModule which is the sender (usually an OtaInterfaceLayer)
     * \param MacAddress source MAC address (usually the MAC of a simulated vehicle)
     * \param MacAddress destination MAC address
     * \param byte_view_range containing the message data
     */
    virtual void sendMessage(const vanetza::MacAddress& source, const vanetza::MacAddress& destination, const vanetza::byte_view_range& data) = 0;

    /**
     * Receives a GeonetPacket which was scheduled by the used scheduler
     * The packet should be transmitted using a OMNeT++ module (eg. the registered OtaInterfaceLayer)
     *
     * \param GeonetPacket which should be sent to the simulation
     */
    virtual void receiveMessage(std::unique_ptr<GeoNetPacket>) = 0;

    /**
     * Should check if a module was registered at the OtaInterface
     */
    virtual bool hasRegisteredModule() = 0;

    virtual ~OtaInterface() = default;
};

} // namespace artery

#endif /* ARTERY_OTA_INTERFACE_H */
