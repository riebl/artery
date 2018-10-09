/*
* Artery V2X Simulation Framework
* Copyright 2014-2018 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/application/Middleware.h"
#include "artery/application/ItsG5PromiscuousService.h"
#include "artery/application/ItsG5Service.h"
#include "artery/networking/Router.h"
#include "artery/utility/PointerCheck.h"
#include "artery/utility/IdentityRegistry.h"
#include "artery/utility/InitStages.h"
#include "artery/utility/FilterRules.h"
#include "inet/common/ModuleAccess.h"

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
        mRouter = inet::getModuleFromPar<Router>(par("routerModule"), findHost());
        mUpdateInterval = par("updateInterval");
        mUpdateMessage = new cMessage("middleware update");
    } else if (stage == InitStages::Self) {
        mFacilities.register_const(&mTimer);
        mFacilities.register_mutable(&mLocalDynamicMap);
        mFacilities.register_const(&mIdentity);
        mFacilities.register_const(&mStationType);

        initializeIdentity(mIdentity);
        initializeServices(InitStages::Self);

        // start update cycle with random jitter to avoid unrealistic node synchronization
        const auto jitter = uniform(SimTime(0, SIMTIME_MS), mUpdateInterval);
        scheduleAt(simTime() + jitter + mUpdateInterval, mUpdateMessage);
    } else if (stage == InitStages::Propagate) {
        emit(artery::IdentityRegistry::updateSignal, &mIdentity);
    }
}

void Middleware::initializeIdentity(Identity& id)
{
    id.geonet = notNullPtr(mRouter)->getAddress();
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
            for (int i = 0; i <= stage; ++i) {
                if (!module->callInitialize(i)) break;
            }

            ItsG5BaseService* service = dynamic_cast<ItsG5BaseService*>(module);
            if (service) {
                cXMLElement* listener = service_cfg->getFirstChildWithTag("listener");
                if (listener && listener->getAttribute("port")) {
                    port_type port = boost::lexical_cast<port_type>(listener->getAttribute("port"));
                    mServices.emplace(service, port);
                    mBtpPortDispatcher.set_non_interactive_handler(vanetza::host_cast<port_type>(port), service);
                } else if (!service->requiresListener()) {
                        mServices.emplace(service, 0);
                } else {
                    auto promiscuous = dynamic_cast<ItsG5PromiscuousService*>(service);
                    if (promiscuous != nullptr) {
                        mServices.emplace(service, 0);
                        mBtpPortDispatcher.add_promiscuous_hook(promiscuous);
                    } else {
                        error("No listener port defined for %s", service_name);
                    }
                }
            } else {
                error("%s is not of type ItsG5BaseService", module_type->getFullName());
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
        update();
    } else {
        error("Middleware cannot handle message '%s'", msg->getFullName());
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

void Middleware::update()
{
    updateServices();
    scheduleAt(simTime() + mUpdateInterval, mUpdateMessage);
}

vanetza::geonet::TransportInterface& Middleware::getTransportInterface()
{
    return mBtpPortDispatcher;
}

void Middleware::updateServices()
{
    mLocalDynamicMap.dropExpired();
    for (auto& kv : mServices) {
        kv.first->trigger();
    }
}

Middleware::port_type Middleware::getPortNumber(const ItsG5BaseService* service) const
{
    port_type port = 0;
    auto it = mServices.find(const_cast<ItsG5BaseService*>(service));
    if (it != mServices.end()) {
        port = it->second;
    }
    return port;
}

void Middleware::requestTransmission(const vanetza::btp::DataRequestB& request, std::unique_ptr<vanetza::DownPacket> packet)
{
    if (mRouter) {
        mRouter->request(request, std::move(packet));
    } else {
        error("Transmission of BTP-B packet requested but router is not available");
    }
}

} // namespace artery

