/*
* Artery V2X Simulation Framework
* Copyright 2019 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_RTCMMOCKSERVICE_H_SRHMWSLO
#define ARTERY_RTCMMOCKSERVICE_H_SRHMWSLO

#include "artery/application/ItsG5Service.h"
#include "artery/networking/PositionProvider.h"
#include <vanetza/units/length.hpp>

namespace artery
{

class RtcmMockService : public artery::ItsG5Service
{
    public:
        virtual ~RtcmMockService();

    protected:
        void initialize() override;
        void handleMessage(omnetpp::cMessage*) override;
        void generatePacket();

    private:
        int mHostId = 0;
        const PositionProvider* mPositionProvider = nullptr;
        omnetpp::cMessage* mTrigger = nullptr;
        omnetpp::SimTime mInterval = omnetpp::SimTime::ZERO;
        bool mGnScf;
        vanetza::units::Length mGnRadius;
        vanetza::geonet::Lifetime mGnLifetime;
        unsigned mDccProfile = 0;
        unsigned mMessageLength = 0;
};

} // namespace artery


#endif /* ARTERY_RTCMMOCKSERVICE_H_SRHMWSLO */
