/*
* Artery V2X Simulation Framework
* Copyright 2014-2018 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_MIDDLEWARE_H_
#define ARTERY_MIDDLEWARE_H_

#include "artery/application/Facilities.h"
#include "artery/application/LocalDynamicMap.h"
#include "artery/application/IndicationInterfaceAdapter.h"
#include "artery/application/PromiscuousHookAdapter.h"
#include "artery/application/McoStrategy.h"
#include "artery/application/NetworkInterface.h"
#include "artery/application/NetworkInterfaceTable.h"
#include "artery/application/ServiceTable.h"
#include "artery/application/StationType.h"
#include "artery/application/Timer.h"
#include "artery/utility/Identity.h"
#include <omnetpp/clistener.h>
#include <omnetpp/csimplemodule.h>
#include <omnetpp/simtime.h>
#include <vanetza/btp/data_request.hpp>
#include <vanetza/btp/port_dispatcher.hpp>
#include <map>
#include <memory>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/any.hpp>

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
        using port_type = uint16_t;
        using PortInfoMap = ServiceTable::PortInfoMap;

        Middleware();
        ~Middleware();

        Facilities& getFacilities() { return mFacilities; }
        const Facilities& getFacilities() const { return mFacilities; }

        PortInfoMap getPortNumber(const ItsG5BaseService*) const;

        const Identity& getIdentity() const { return mIdentity; }
        const StationType& getStationType() const { return mStationType; }

        vanetza::geonet::TransportInterface& getTransportInterface(artery::Router& router);
        NetworkInterface& registerNetworkInterface(Router& router, IDccEntity& entity);

        void requestTransmission(const vanetza::btp::DataRequestB&, std::unique_ptr<vanetza::DownPacket>, boost::optional<NetworkInterface&> = boost::none);

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

        std::set<ItsG5BaseService*> mServices;

    private:
        NetworkInterfaceTable mNetworkInterfaceTable;
        ServiceTable mServiceTable;

        std::unique_ptr<McoStrategy> mMcoStrategy;
};

} // namespace artery

#endif
