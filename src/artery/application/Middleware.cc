/*
* Artery V2X Simulation Framework
* Copyright 2014-2019 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/application/Middleware.h"
#include "artery/application/ItsG5PromiscuousService.h"
#include "artery/application/ItsG5Service.h"
#include "artery/application/XmlMultiChannelPolicy.h"
#include "artery/networking/Router.h"
#include "artery/utility/Channel.h"
#include "artery/utility/PointerCheck.h"
#include "artery/utility/IdentityRegistry.h"
#include "artery/utility/InitStages.h"
#include "artery/utility/FilterRules.h"
#include "inet/common/ModuleAccess.h"

using namespace omnetpp;

namespace artery
{

Define_Module(Middleware)

namespace
{

ChannelNumber getChannel(const omnetpp::cXMLElement* cfg)
{
    ChannelNumber channel = 0;

    const char* ch_attr = cfg->getAttribute("channel");
    if (ch_attr) {
        channel = parseChannelNumber(ch_attr);
    }

    // fall back to control channel for backward-compatibility
    if (channel == 0) {
        channel = channel::CCH;
    }

    return channel;
}

} // namespace

Middleware::Middleware() : mLocalDynamicMap(mTimer)
{
}

Middleware::~Middleware()
{
    cancelAndDelete(mUpdateMessage);
}

int Middleware::numInitStages() const
{
    return InitStages::Total;
}

void Middleware::initialize(int stage)
{
    if (stage == InitStages::Prepare) {
        mTimer.setTimebase(par("datetime"));
        mUpdateInterval = par("updateInterval");
        mUpdateMessage = new cMessage("middleware update");
        mIdentity.host = findHost();
        mIdentity.host->subscribe(Identity::changeSignal, this);
        mMultiChannelPolicy.reset(new XmlMultiChannelPolicy(par("mcoPolicy").xmlValue()));
    } else if (stage == InitStages::Self) {
        mFacilities.register_const(&mTimer);
        mFacilities.register_mutable(&mLocalDynamicMap);
        mFacilities.register_const(&mIdentity);
        mFacilities.register_const(&mStationType);
        mFacilities.register_const(mMultiChannelPolicy.get());
        mFacilities.register_const(&mNetworkInterfaceTable);

        initializeServices(InitStages::Self);

        // start update cycle with random jitter to avoid unrealistic node synchronization
        const auto jitter = uniform(SimTime(0, SIMTIME_MS), mUpdateInterval);
        scheduleAt(simTime() + jitter + mUpdateInterval, mUpdateMessage);
    } else if (stage == InitStages::Propagate) {
        emit(artery::IdentityRegistry::updateSignal, &mIdentity);
    }
}

void Middleware::initializeServices(int stage)
{
    cXMLElement* config = par("services").xmlValue();
    for (cXMLElement* service_cfg : config->getChildrenByTagName("service")) {
        cModuleType* module_type = cModuleType::get(service_cfg->getAttribute("type"));

        cXMLElement* service_filters = service_cfg->getFirstChildWithTag("filters");
        bool service_applicable = true;
        if (service_filters) {
            artery::FilterRules rules(getRNG(0), mIdentity);
            service_applicable = rules.applyFilterConfig(*service_filters);
        }

        if (service_applicable) {
            const char* service_name = service_cfg->getAttribute("name") ?
                service_cfg->getAttribute("name") : module_type->getName();
            cModule* module = module_type->create(service_name, this);
            module->finalizeParameters();
            module->buildInside();
            module->scheduleStart(simTime());
            // defer final module initialisation until service is attached to middleware

            ItsG5BaseService* service = dynamic_cast<ItsG5BaseService*>(module);
            if (service) {
                unsigned ports = 0;
                unsigned channels = 0;
                auto promiscuous = dynamic_cast<ItsG5PromiscuousService*>(service);

                for (const cXMLElement* listener : service_cfg->getChildrenByTagName("listener")) {
                    if (listener->getAttribute("port")) {
                        auto port = boost::lexical_cast<PortNumber>(listener->getAttribute("port"));
                        TransportDescriptor td = std::forward_as_tuple(getChannel(listener), port);
                        mTransportDispatcher.addListener(service, td);
                        service->addTransportDescriptor(td);
                        ++ports;
                    } else if (promiscuous && listener->getAttribute("channel")) {
                        ChannelNumber channel = getChannel(listener);
                        mTransportDispatcher.addPromiscuousListener(promiscuous, channel);
                        ++channels;
                    }
                }

                // ensure that ordinary ITS-G5 services are listening to at least port
                if (ports == 0 && !promiscuous && service->requiresListener()) {
                    error("Listening ports are required for %s but none have been specified", module_type->getFullName());
                }

                // promiscuous ITS-G5 services grab packets from CCH by default if not specified otherwise
                if (promiscuous && channels == 0) {
                    mTransportDispatcher.addPromiscuousListener(promiscuous, channel::CCH);
                }

                mServices.emplace(service);
            } else {
                error("%s is not of type ItsG5BaseService", module_type->getFullName());
            }

            // finalize module initialization now
            for (int i = 0; i <= stage; ++i) {
                if (!module->callInitialize(i)) break;
            }
        }
    }
}

void Middleware::finish()
{
    emit(artery::IdentityRegistry::removeSignal, &mIdentity);
}

void Middleware::handleMessage(cMessage *msg)
{
    if (msg == mUpdateMessage) {
        updateServices();
    } else {
        error("Middleware cannot handle message '%s'", msg->getFullName());
    }
}

void Middleware::receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t signal, long changes, omnetpp::cObject* obj)
{
    if (signal == Identity::changeSignal) {
        auto identity = check_and_cast<Identity*>(obj);
        if (mIdentity.update(*identity, changes)) {
            emit(artery::IdentityRegistry::updateSignal, &mIdentity);
        }
    }
}

cModule* Middleware::findHost()
{
    return inet::getContainingNode(this);
}

void Middleware::setStationType(const StationType& type)
{
    mStationType = type;
}

void Middleware::registerNetworkInterface(std::shared_ptr<NetworkInterface> ifc)
{
    mNetworkInterfaceTable.insert(ifc);
}

void Middleware::updateServices()
{
    mLocalDynamicMap.dropExpired();
    for (auto& service : mServices) {
        service->trigger();
    }
    scheduleAt(simTime() + mUpdateInterval, mUpdateMessage);
}

void Middleware::requestTransmission(const vanetza::btp::DataRequestB& request,
        std::unique_ptr<vanetza::DownPacket> packet, const NetworkInterface& netifc)
{
    Enter_Method("requestTransmission");
    netifc.getRouter().request(request, std::move(packet));
}

void Middleware::requestTransmission(const vanetza::btp::DataRequestB& request, std::unique_ptr<vanetza::DownPacket> packet)
{
    Enter_Method("requestTransmission");

    auto channels = mMultiChannelPolicy->allChannels(request.gn.its_aid);
    if (channels.empty()) {
        EV_WARN << "No channel found for ITS-AID " << request.gn.its_aid << "\n";
    }

    unsigned pass = 0;
    for (ChannelNumber channel : channels) {
        auto netifc = mNetworkInterfaceTable.select(channel);
        if (netifc) {
            ++pass;
            if (channels.size() > pass) {
                // duplicate packet for all but last network interface
                netifc->getRouter().request(request, vanetza::duplicate(*packet));
            } else {
                // last network interface -> pass "original" packet
                netifc->getRouter().request(request, std::move(packet));
            }
        } else {
            EV_ERROR << "No network interface operating on channel " <<  channel << "\n";
        }
    }

    if (pass == 0) {
        EV_ERROR << "ITS-AID " << request.gn.its_aid << " packet lost in Middleware\n";
    } else {
        EV_DETAIL << "ITS-AID " << request.gn.its_aid << " packet passed to " << pass << " network interfaces\n";
    }
}

} // namespace artery

