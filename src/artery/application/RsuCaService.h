/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2020 Raphael Riebl et al.
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_RSUCASERVICE_H_
#define ARTERY_RSUCASERVICE_H_

#include "artery/application/ItsG5BaseService.h"
#include "artery/utility/Channel.h"
#include <vanetza/asn1/cam.hpp>
#include <omnetpp/simtime.h>
#include <boost/optional/optional.hpp>
#include <list>

namespace artery
{

class GeoPosition;
class Identity;
class LocalDynamicMap;
class NetworkInterfaceTable;
class Timer;

class RsuCaService : public ItsG5BaseService
{
    public:
        void initialize() override;
        void indicate(const vanetza::btp::DataIndication&, std::unique_ptr<vanetza::UpPacket>) override;
        void trigger() override;

        struct ProtectedCommunicationZone
        {
            boost::optional<ProtectedZoneID_t> id;
            ProtectedZoneType_t type = ProtectedZoneType_permanentCenDsrcTolling;
            double latitude_deg = 0.0;
            double longitude_deg = 0.0;
            unsigned radius_m = 0;
        };

        static std::list<ProtectedCommunicationZone> parseProtectedCommunicationZones(omnetpp::cXMLElement*);

    private:
        void sendCam();
        vanetza::asn1::Cam createMessage() const;

        ChannelNumber mPrimaryChannel = channel::CCH;
        const NetworkInterfaceTable* mNetworkInterfaceTable = nullptr;
        const Timer* mTimer = nullptr;
        const Identity* mIdentity = nullptr;
        const GeoPosition* mGeoPosition = nullptr;
        LocalDynamicMap* mLocalDynamicMap = nullptr;
        omnetpp::SimTime mGenerationInterval;
        omnetpp::SimTime mLastCamTimestamp;
        std::list<ProtectedCommunicationZone> mProtectedCommunicationZones;
};

} // namespace artery

#endif /* ARTERY_RSUCASERVICE_H_ */
