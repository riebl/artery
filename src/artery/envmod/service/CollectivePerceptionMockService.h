/*
* Artery V2X Simulation Framework
* Copyright 2019 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_COLLECTIVEPERCEPTIONMOCKSERVICE_H_V08YXH9S
#define ARTERY_COLLECTIVEPERCEPTIONMOCKSERVICE_H_V08YXH9S

#include "artery/envmod/service/CollectivePerceptionMockMessage.h"
#include "artery/application/ItsG5Service.h"
#include "artery/networking/PositionProvider.h"
#include <unordered_set>
#include <vector>

namespace artery
{

class CollectivePerceptionMockService : public ItsG5Service
{
    public:
        virtual ~CollectivePerceptionMockService();

    protected:
        int numInitStages() const override;
        void initialize(int stage) override;
        void trigger() override;
        void handleMessage(omnetpp::cMessage*) override;
        void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, omnetpp::cObject*, omnetpp::cObject*) override;
        void generatePacket();
        void indicate(const vanetza::btp::DataIndication&, omnetpp::cPacket*) override;

    private:
        int mHostId = 0;
        const PositionProvider* mPositionProvider = nullptr;
        const LocalEnvironmentModel* mEnvironmentModel = nullptr;
        omnetpp::cMessage* mTrigger = nullptr;
        bool mGenerateAfterCam;
        omnetpp::SimTime mCpmOffset;
        omnetpp::SimTime mCpmInterval;
        omnetpp::SimTime mFovInterval = omnetpp::SimTime::ZERO;
        omnetpp::SimTime mFovLast = omnetpp::SimTime::ZERO;
        std::vector<CollectivePerceptionMockMessage::FovContainer> mFovContainers;
        std::unordered_set<const Sensor*> mSensors;
        unsigned mDccProfile = 0;
        unsigned mLengthHeader = 0;
        unsigned mLengthFovContainer = 0;
        unsigned mLengthObjectContainer = 0;
};

} // namespace artery

#endif /* ARTERY_COLLECTIVEPERCEPTIONMOCKSERVICE_H_V08YXH9S */
