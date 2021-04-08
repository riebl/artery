/*
* Artery V2X Simulation Framework
* Copyright 2020 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_PERIODICLOADSERVICE_H_
#define ARTERY_PERIODICLOADSERVICE_H_

#include "artery/application/ItsG5Service.h"
#include "artery/application/NetworkInterface.h"

namespace artery
{

class PeriodicLoadService : public ItsG5Service
{
    public:
        PeriodicLoadService();
        ~PeriodicLoadService();

        void indicate(const vanetza::btp::DataIndication&, omnetpp::cPacket*, const NetworkInterface&) override;
        void trigger() override;

    protected:
        void initialize() override;
        void handleMessage(omnetpp::cMessage*) override;
        void scheduleTransmission();
        void generateTransmissionRequest();

    private:
        omnetpp::cMessage* mTrigger;
        std::uint32_t mAppId;
};

} // namespace artery

#endif /* ARTERY_PERIODICLOADSERVICE_H_ */
