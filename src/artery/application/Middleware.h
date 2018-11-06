/*
* Artery V2X Simulation Framework
* Copyright 2014-2019 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_MIDDLEWARE_H_
#define ARTERY_MIDDLEWARE_H_

#include "artery/application/Facilities.h"
#include "artery/application/LocalDynamicMap.h"
#include "artery/application/MultiChannelPolicy.h"
#include "artery/application/NetworkInterface.h"
#include "artery/application/NetworkInterfaceTable.h"
#include "artery/application/StationType.h"
#include "artery/application/Timer.h"
#include "artery/application/TransportDispatcher.h"
#include "artery/utility/Identity.h"
#include <omnetpp/clistener.h>
#include <omnetpp/csimplemodule.h>
#include <omnetpp/simtime.h>
#include <vanetza/btp/data_request.hpp>
#include <vanetza/btp/port_dispatcher.hpp>
#include <memory>
#include <set>

namespace artery
{

// forward declarations
class ItsG5BaseService;
class Router;

/**
 * Middleware providing a runtime context for services.
 */
class Middleware : public omnetpp::cSimpleModule, public omnetpp::cListener
{
    public:
        Middleware();
        ~Middleware();

        Facilities& getFacilities() { return mFacilities; }
        const Facilities& getFacilities() const { return mFacilities; }

        const Identity& getIdentity() const { return mIdentity; }
        const StationType& getStationType() const { return mStationType; }
        const TransportDispatcher& getTransportDispatcher() const { return mTransportDispatcher; }

        /**
         * Register a network interface at middleware's network interface table.
         * Only previously registered interfaces are recognised by services.
         *
         * \param ifc network interface
         */
        void registerNetworkInterface(std::shared_ptr<NetworkInterface>);

        void requestTransmission(const vanetza::btp::DataRequestB&, std::unique_ptr<vanetza::DownPacket>);
        void requestTransmission(const vanetza::btp::DataRequestB&, std::unique_ptr<vanetza::DownPacket>, const NetworkInterface&);

    protected:
        // cSimpleModule
        int numInitStages() const override;
        void initialize(int stage) override;
        void finish() override;
        void handleMessage(omnetpp::cMessage* msg) override;

        // cListener
        void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, long, omnetpp::cObject*) override;

        omnetpp::cModule* findHost();
        void setStationType(const StationType&);

    private:
        void updateServices();
        void initializeServices(int stage);

        omnetpp::SimTime mUpdateInterval;
        omnetpp::cMessage* mUpdateMessage = nullptr;
        Timer mTimer;
        Identity mIdentity;
        LocalDynamicMap mLocalDynamicMap;
        Facilities mFacilities;
        StationType mStationType;

        NetworkInterfaceTable mNetworkInterfaceTable;
        TransportDispatcher mTransportDispatcher;
        std::unique_ptr<MultiChannelPolicy> mMultiChannelPolicy;
        std::set<ItsG5BaseService*> mServices;
};

} // namespace artery

#endif
