#ifndef ARTERY_DCCENTITYBASE_H_VLQQNLKF
#define ARTERY_DCCENTITYBASE_H_VLQQNLKF

#include "artery/networking/AccessInterface.h"
#include "artery/networking/IDccEntity.h"
#include <vanetza/dcc/flow_control.hpp>
#include <vanetza/geonet/dcc_information_sharing.hpp>
#include <omnetpp/clistener.h>
#include <omnetpp/csimplemodule.h>
#include <memory>

namespace artery
{

class RadioDriverBase;
class Router;

class DccEntityBase : public IDccEntity, public omnetpp::cSimpleModule, public omnetpp::cListener
{
public:
    // cSimpleModule
    int numInitStages() const override;
    void initialize(int stage) override;
    void finish() override;

    // cListener
    void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, double, omnetpp::cObject*) override;

    // IDccEntity
    vanetza::dcc::ChannelProbeProcessor* getChannelProbeProcessor() override { return mCbrProcessor.get(); }
    vanetza::dcc::RequestInterface* getRequestInterface() override { return mFlowControl.get(); }
    vanetza::geonet::DccFieldGenerator* getGeonetFieldGenerator() override { return mNetworkEntity.get(); }
    void reportLocalChannelLoad(vanetza::dcc::ChannelLoad) override;

protected:
    virtual void initializeNetworkEntity(const std::string&);
    virtual void initializeChannelProbeProcessor(const std::string&);
    virtual void initializeTransmitRateControl() = 0;

    virtual void onLocalCbr(vanetza::dcc::ChannelLoad);
    virtual void onGlobalCbr(vanetza::dcc::ChannelLoad) = 0;
    virtual vanetza::dcc::TransmitRateControl* getTransmitRateControl() = 0;

    std::unique_ptr<vanetza::dcc::ChannelProbeProcessor> mCbrProcessor;
    std::unique_ptr<vanetza::geonet::DccInformationSharing> mNetworkEntity;
    std::unique_ptr<vanetza::dcc::FlowControl> mFlowControl;
    vanetza::dcc::ChannelLoad mTargetCbr;
    Router* mRouter;
    vanetza::Runtime* mRuntime;
    std::unique_ptr<AccessInterface> mAccessInterface;
};

} // namespace artery

#endif /* ARTERY_DCCENTITYBASE_H_VLQQNLKF */

