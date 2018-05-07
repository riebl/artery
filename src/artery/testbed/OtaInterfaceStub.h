/*
 * Artery V2X Simulation Framework
 * Copyright 2017-2018 Christina Obermaier
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_OTA_INTERFACE_STUB_H
#define ARTERY_OTA_INTERFACE_STUB_H

#include "artery/testbed/OtaInterface.h"
#include <omnetpp/csimplemodule.h>

namespace artery
{

/**
 * The OtaInterfaceStub provides a implementation of the OtaInterface which acts as an stub to work without any hardware connected.
 * It implements all necessary methods, mos of them doing nothing.
 * It can be used to compile the testbed without any external library available (like the S.E.A. API required by the OtaInterfaceUsrp).
 */
class OtaInterfaceStub : public OtaInterface, public omnetpp::cSimpleModule
{
public:
    void initialize() override;
    void registerModule(OtaInterfaceLayer*) override;
    void unregisterModule() override;
    void sendMessage(const vanetza::MacAddress&, const vanetza::MacAddress&, const vanetza::byte_view_range&) override;
    void receiveMessage(std::unique_ptr<GeoNetPacket>) override;
    bool hasRegisteredModule() override { return mRegisteredModule != nullptr; }

private:
    OtaInterfaceLayer* mRegisteredModule = nullptr;
};

} // namespace artery

#endif /* ARTERY_OTA_INTERFACE_STUB_H */
