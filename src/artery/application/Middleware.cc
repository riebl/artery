/*
* Artery V2X Simulation Framework
* Copyright 2014-2018 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/application/Middleware.h"
#include "artery/application/ItsG5PromiscuousService.h"
#include "artery/application/ItsG5Service.h"
#include "artery/networking/Router.h"
#include "artery/networking/FsmDccEntity.h"
#include "artery/utility/PointerCheck.h"
#include "artery/utility/IdentityRegistry.h"
#include "artery/utility/InitStages.h"
#include "artery/utility/FilterRules.h"
#include "inet/common/ModuleAccess.h"

using namespace omnetpp;

namespace artery
{

Define_Module(Middleware)

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
    } else if (stage == InitStages::Self) {
        mFacilities.register_const(&mTimer);
        mFacilities.register_mutable(&mLocalDynamicMap);
        mFacilities.register_const(&mIdentity);
        mFacilities.register_const(&mStationType);

        mMcoStrategy.reset(new McoStrategy(mNetworkInterfaceTable));
        mFacilities.register_mutable(mMcoStrategy.get());
        mFacilities.register_mutable(&mNetworkInterfaceTable);
    } else if (stage == InitStages::Services) {
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
                for (cXMLElement *listener = service_cfg->getFirstChildWithTag("listener");
                     listener != nullptr;
                     listener = listener->getNextSiblingWithTag("listener"))
                {
                    if (listener && listener->getAttribute("port")) {
                        port_type port = boost::lexical_cast<port_type>(listener->getAttribute("port"));
                        auto port_host = vanetza::host_cast<port_type>(port);
                        Channel channel = boost::lexical_cast<Channel>(listener->getAttribute("channel"));
                        vanetza::ItsAid aid = boost::lexical_cast<Channel>(listener->getAttribute("aid"));

                        mServices.emplace(service);
                        mMcoStrategy->add(aid, channel);

                        auto interfaces = mNetworkInterfaceTable.getInterfaceByChannel(channel);
                        for (auto& it : boost::make_iterator_range(interfaces)) {
                            auto& interface = *it;
                            auto adapter = new IndicationInterfaceAdapter(interface, (IndicationInterface*)service);

                            interface.btpPortDispatcher.set_non_interactive_handler(port_host, adapter);
                            mServiceTable.insert(service, port, interface, adapter);
                        }
                    } else if (!service->requiresListener()) {
                        mServices.emplace(service);
                    } else if (listener && listener->getAttribute("channel")) {
                        auto promiscuous = dynamic_cast<ItsG5PromiscuousService*>(service);
                        if (promiscuous != nullptr) {
                            Channel channel = boost::lexical_cast<Channel>(listener->getAttribute("channel"));
                            vanetza::ItsAid aid = boost::lexical_cast<Channel>(listener->getAttribute("aid"));

                            mServices.emplace(service);
                            mMcoStrategy->add(aid, channel);

                            auto interfaces = mNetworkInterfaceTable.getInterfaceByChannel(channel);
                            for (auto& it : interfaces) {
                                auto& interface = *it;
                                auto adapter = new PromiscuousHookAdapter(interface, (PromiscuousHook*)promiscuous);

                                interface.btpPortDispatcher.add_promiscuous_hook(adapter);
                                mServiceTable.insert(service, interface, adapter);
                            }
                        } else {
                            error("No listener port defined for %s", service_name);
                        }
                    }
                }
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


vanetza::geonet::TransportInterface& Middleware::getTransportInterface(artery::Router& router)
{
    auto ifc = mNetworkInterfaceTable.getInterfaceByRouter(&router);
    if (ifc.begin() != ifc.end()) {
        return ifc.begin()->get()->btpPortDispatcher;
    } else {
        throw new cRuntimeError("Router not found");
    }
}

NetworkInterface& Middleware::registerNetworkInterface(Router& router, IDccEntity& entity)
{
    mNetworkInterfaceTable.insert(&router, &entity);
    auto ifc = mNetworkInterfaceTable.getInterfaceByRouter(&router).begin();
    return *ifc->get();
}

void Middleware::updateServices()
{
    mLocalDynamicMap.dropExpired();
    for (auto& service : mServices) {
        service->trigger();
    }
    scheduleAt(simTime() + mUpdateInterval, mUpdateMessage);
}

Middleware::PortInfoMap Middleware::getPortNumber(const ItsG5BaseService* service) const
{
    return mServiceTable.getPortsForService(service);
}

void Middleware::requestTransmission(const vanetza::btp::DataRequestB& request, std::unique_ptr<vanetza::DownPacket> packet, boost::optional<NetworkInterface&> interface)
{
    Enter_Method("requestTransmission");
    if (interface) {
        auto router = interface->router;
        router->request(request, std::move(packet));
    } else {
        auto ifcs = mMcoStrategy->choose(request.gn.its_aid);
        for (auto& ifc : ifcs) {
            auto router = ifc->router;
            std::unique_ptr<vanetza::DownPacket> pkt(new vanetza::DownPacket(*packet));
            router->request(request, std::move(pkt));
        }
    }
}

} // namespace artery

