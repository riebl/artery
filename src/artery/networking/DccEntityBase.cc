#include "artery/application/Middleware.h"
#include "artery/networking/DccEntityBase.h"
#include "artery/networking/Router.h"
#include "artery/networking/Runtime.h"
#include "artery/nic/RadioDriverBase.h"
#include "artery/utility/InitStages.h"
#include "artery/utility/PointerCheck.h"
#include "inet/common/ModuleAccess.h"
#include <vanetza/dcc/hooked_channel_probe_processor.hpp>
#include <vanetza/dcc/smoothing_channel_probe_processor.hpp>

using namespace omnetpp;

namespace artery
{

void DccEntityBase::initialize(int stage)
{
    if (stage == InitStages::Prepare) {
        mTargetCbr = vanetza::dcc::ChannelLoad { par("targetCbr") };
        mRouter = inet::getModuleFromPar<Router>(par("routerModule"), this);
        mRuntime = inet::getModuleFromPar<Runtime>(par("runtimeModule"), this);

        auto radioDriver = inet::getModuleFromPar<RadioDriverBase>(par("radioDriverModule"), this);
        radioDriver->subscribe(RadioDriverBase::ChannelLoadSignal, this);
        mAccessInterface.reset(new AccessInterface(gate("radioDriverData")));
        ASSERT(gate("radioDriverData")->pathContains(radioDriver));
    } else if (stage == InitStages::Self) {
        initializeChannelProbeProcessor(par("ChannelProbeProcessor"));
        initializeNetworkEntity(par("NetworkEntity"));
        initializeTransmitRateControl();
        auto trc = notNullPtr(getTransmitRateControl());
        mFlowControl.reset(new vanetza::dcc::FlowControl(*mRuntime, *trc, *mAccessInterface));
        mFlowControl->queue_length(par("queueLength"));
    }
}

int DccEntityBase::numInitStages() const
{
    return InitStages::Total;
}

void DccEntityBase::finish()
{
    // free those objects before runtime vanishes
    mFlowControl.reset();
    mNetworkEntity.reset();
    mCbrProcessor.reset();
}

void DccEntityBase::receiveSignal(cComponent*, simsignal_t signal, double value, cObject*)
{
    if (signal == RadioDriverBase::ChannelLoadSignal) {
        ASSERT(value >= 0.0 && value <= 1.0);
        vanetza::dcc::ChannelLoad cl { value };
        reportLocalChannelLoad(cl);
    }
}

void DccEntityBase::initializeNetworkEntity(const std::string& name)
{
    ASSERT(mRuntime);
    ASSERT(mRouter);

    if (name.empty() || name == "Null") {
        mNetworkEntity.reset(nullptr);
    } else if (name == "TwoHopChannelLoad") {
        using namespace vanetza;
        const auto& loct = mRouter->getLocationTable();
        const UnitInterval delay { uniform(0.0, 1.0) };
        mNetworkEntity.reset(new geonet::DccInformationSharing(*mRuntime, loct, mTargetCbr, delay));
        mNetworkEntity->on_global_cbr_update = [this](const geonet::CbrAggregator& cbr) {
            this->onGlobalCbr(cbr.get_global_cbr());
        };
    } else {
        error("Unknown DCC network entity \"%s\"", name.c_str());
    }
}

void DccEntityBase::initializeChannelProbeProcessor(const std::string& name)
{
    using namespace vanetza::dcc;

    std::unique_ptr<HookedChannelProbeProcessor> processor;
    if (name == "Local") {
        processor.reset(new HookedChannelProbeProcessor());
    } else if (name == "SmoothingLocal") {
        processor.reset(new SmoothingChannelProbeProcessor());
    } else {
        error("Unknown DCC channel probe processor \"%s\"", name.c_str());
    }

    processor->on_indication = std::bind(&DccEntityBase::onLocalCbr, this, std::placeholders::_1);
    mCbrProcessor = std::move(processor);
}

void DccEntityBase::reportLocalChannelLoad(vanetza::dcc::ChannelLoad cbr)
{
    mCbrProcessor->indicate(cbr);
}

void DccEntityBase::onLocalCbr(vanetza::dcc::ChannelLoad cbr)
{
    if (mNetworkEntity) {
        mNetworkEntity->update_local_cbr(cbr);
    } else {
        onGlobalCbr(cbr);
    }
}

} // namespace artery
