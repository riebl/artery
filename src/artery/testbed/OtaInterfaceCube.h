/*
 * Artery V2X Simulation Framework
 * Copyright 2024 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_OTAINTERFACE_CUBE_H
#define ARTERY_OTAINTERFACE_CUBE_H

#include "artery/testbed/OtaIndicationQueue.h"
#include "artery/testbed/OtaInterface.h"
#include <omnetpp/csimplemodule.h>
#include <memory>

namespace artery
{

class CubeConnection;
class OtaInterfaceLayer;

class OtaInterfaceCube : public omnetpp::cSimpleModule, public OtaInterface
{
public:
    void registerModule(OtaInterfaceLayer*) override;
    void unregisterModule() override;
    void sendMessage(const vanetza::MacAddress& source, const vanetza::MacAddress& destination, const vanetza::byte_view_range& data) override;
    void receiveMessage(std::unique_ptr<GeoNetPacket>) override;
    bool hasRegisteredModule() override;

protected:
    int numInitStages() const override;
    void initialize(int) override;
    void finish() override;

private:
    OtaInterfaceLayer* mRegisteredModule = nullptr;
    std::shared_ptr<OtaIndicationQueue> mOtaIndicationQueue;
    std::unique_ptr<CubeConnection> mCubeConnection;

    unsigned mPacketsForwarded = 0; /*< packets forwarded from simulation to CUBE */
    unsigned mPacketsInjected = 0; /*< packets injected into simulation from CUBE */
};

} // namespace artery

#endif /* ARTERY_OTAINTERFACE_CUBE_H */
