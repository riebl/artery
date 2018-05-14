/*
 * Artery V2X Simulation Framework
 * Copyright 2017-2018 Christina Obermaier
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_OTAINTERFACE_USRP_H
#define ARTERY_OTAINTERFACE_USRP_H

#include "artery/testbed/GpsdServer.h"
#include "artery/testbed/OtaIndicationQueue.h"
#include "artery/testbed/OtaInterface.h"
#include "artery/testbed/UsrpConnection.h"
#include <omnetpp/csimplemodule.h>
#include <omnetpp/clistener.h>
#include <sea_v2x/ma_unitdata_indication.h>
#include <memory>

namespace artery
{

class OtaInterfaceLayer;

class OtaInterfaceUsrp : public omnetpp::cSimpleModule, public omnetpp::cListener, public OtaInterface
{
public:
    /**
     * Register Physical Twin module at the Ota OtaInterface
     * Throws cRuntimeError if a second modules tries to register
     * Must be called from the OtaInterfaceLayer when joining simulation (eg. in the intialize() method)
     */
    void registerModule(OtaInterfaceLayer*) override;

    /**
     * Unregisters the registeredModule and closes the the GPSD socket if it was opened
     * Must be called from the OtaInterfaceLayer when leaving the simulation (eg. in the finish() method)
     */
    void unregisterModule() override;

    /**
     * Sends message from Physical Twin module to the USRP Device
     * Throws cRuntimeError if message is from a not registered module
     *
     * \param source MAC address of the sending node
     * \param destination MAC address of the receiving module, usually the MAC address of the device under test
     * \param data Byte range which should be transmitted over the air
     */
    void sendMessage(const vanetza::MacAddress& source, const vanetza::MacAddress& destination, const vanetza::byte_view_range& data) override;

    /**
     * Receives a GeonetPacket which was scheduled by the ThreadSafeScheduler
     *
     * \param GeonetPacket which should be sent to the simulation
     */
    void receiveMessage(std::unique_ptr<GeoNetPacket>) override;

    /**
     * Places a MA_UNITDATA_indication in the OtaIndicationQueue
     * This method is called from the USRP Connection if a MA_UNITDATA_indication was received by the USRP
     * -> called when a message was sent by the DUT and received from the USRP
     */
    virtual void notifyQueue(const sea_v2x::MA_UNITDATA_indication&);

    /**
     * Tests if the physical twin is registered at the OtaInterface
     *
     * \return true if physical twin is registered
     */
    bool hasRegisteredModule() override;

protected:
    int numInitStages() const override;
    void initialize(int) override;
    void finish() override;
    void handleMessage(omnetpp::cMessage*) override;

    /**
     * Receives traci.step signal and fetches the new reachable vehicle list from the TestbedRadio module
     */
    void receiveSignal(omnetpp::cComponent* source, omnetpp::simsignal_t, const omnetpp::simtime_t&, omnetpp::cObject*) override;

private:
    virtual void testUsrpConnection();

    bool mFakeMode = false;
    OtaInterfaceLayer* mRegisteredModule = nullptr;
    std::unique_ptr<UsrpConnection> mConnection;
    std::shared_ptr<OtaIndicationQueue> mOtaIndicationQueue;

    omnetpp::cMessage* mGpsdTimer = nullptr;
    omnetpp::simtime_t mGpsdUpdateRate;
    std::unique_ptr<GpsdServer> mGpsdServer;

    int mMessagesToDut = 0;
    int mMessagesFromDut = 0;
};

} // namespace artery

#endif /* OTAINTERFACE_USRP_H */
