/*
 * Artery V2X Simulation Framework
 * Copyright 2017-2018 Christina Obermaier
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_USRPCONNECTION_H
#define ARTERY_USRPCONNECTION_H

#include <vanetza/common/byte_view.hpp>
#include <vanetza/net/mac_address.hpp>
#include <sea_v2x/v2x.h>
#include <mutex>
#include <string>

namespace artery
{

class OtaInterfaceUsrp;

class UsrpConnection
{
public:
    struct SeaConnectionParams {
        std::string hcmIP;          //IP from HCM machine
        uint16_t hcmPort;           //listen port from HCM machine
        uint16_t listeningPort;     //listen port form this machine
        int connectTimeout;
    };

    UsrpConnection(const SeaConnectionParams&, OtaInterfaceUsrp*);
    ~UsrpConnection();

    virtual bool isConnectionOk() { return mConnectionOk; }

    /**
     * Configure USRP settings via remote API
     */
    virtual void configureUsrp();

    /**
     * Adds needed handler functions to receive responses from the USRP
     */
    virtual void setHandlerFunctions(sea_v2x::V2X&);

    /**
     * Creates MA_UNITDATA_request according to the given information
     *
     * \param source MAC address of the sending node
     * \param destination MAC address of the receiving module, usually the MAC address of the device under test
     * \param data byte range which should be transmitted over the air
     */
    virtual void sendPacket(const vanetza::MacAddress& source, const vanetza::MacAddress& destination, const vanetza::byte_view_range& data);

    /**
     * Shuts down the sea_v2x connection
     * Can be used to close the sea_v2x socket
     */
    virtual void shutDownConnection();

private:
     /**
     * Connects to USRP Device
     */
    virtual void connectToUsrp();

    bool mConnectionOk = false;
    std::unique_ptr<sea_v2x::V2X> mV2X;
    SeaConnectionParams mConnectionParams;
    OtaInterfaceUsrp* mOtaInterfaceCallback;
    std::mutex mMutex;
};

} // namespace artery

#endif /* ARTERY_USRPCONNECTION_H */
