/*
* Artery V2X Simulation Framework
* Copyright 2020 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_GBCMOCKSERVICE_H_EOU21TYG
#define ARTERY_GBCMOCKSERVICE_H_EOU21TYG

#include "artery/application/ItsG5Service.h"
#include "artery/networking/PositionProvider.h"
#include <vanetza/units/length.hpp>

namespace artery
{

class GbcMockService : public artery::ItsG5Service
{
    public:
        virtual ~GbcMockService();

    protected:
        void initialize() override;
        void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, omnetpp::cObject*, omnetpp::cObject*) override;
        void handleMessage(omnetpp::cMessage*) override;
        void indicate(const vanetza::btp::DataIndication&, omnetpp::cPacket*) override;
        void generatePacket();

    private:
        int mHostId = 0;
        const PositionProvider* mPositionProvider = nullptr;
        omnetpp::cMessage* mTrigger = nullptr;
        bool mGnScf;
        vanetza::units::Length mGnRadius;
        vanetza::geonet::Lifetime mGnLifetime;
        vanetza::units::Duration mGnRepetitionInterval;
        vanetza::units::Duration mGnRepetitionMaximum;
        unsigned mDccProfile = 0;
        unsigned mMessageLength = 0;
        int mPacketCounter = 0;
        int mPacketLimit = -1;
};

} // namespace artery

#endif /* ARTERY_GBCMOCKSERVICE_H_EOU21TYG */
