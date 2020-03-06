#ifndef ARTERY_GTUPOSITIONPROVIDER_H_6GJVQCFE
#define ARTERY_GTUPOSITIONPROVIDER_H_6GJVQCFE

#include "artery/networking/PositionFixObject.h"
#include "artery/networking/Runtime.h"
#include <omnetpp/clistener.h>
#include <omnetpp/csimplemodule.h>
#include <vanetza/common/position_provider.hpp>

namespace artery
{

class GtuInetMobility;

class GtuPositionProvider :
    public omnetpp::cSimpleModule, public omnetpp::cListener,
    public vanetza::PositionProvider
{
public:
    void initialize(int stage) override;
    int numInitStages() const override;
    void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, omnetpp::cObject*, omnetpp::cObject*) override;
    const vanetza::PositionFix& position_fix() override { return mPositionFix; }

private:
    void updatePositionFix();

    PositionFixObject mPositionFix;
    Runtime* mRuntime = nullptr;
    GtuInetMobility* mMobility = nullptr;
};

} // namespace artery

#endif /* ARTERY_GTUPOSITIONPROVIDER_H_6GJVCFE */
