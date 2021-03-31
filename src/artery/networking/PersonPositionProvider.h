#ifndef ARTERY_PERSONPOSITIONPROVIDER_H_DGOSJECM
#define ARTERY_PERSONPOSITIONPROVIDER_H_DGOSJECM

#include "artery/networking/PositionFixObject.h"
#include "artery/networking/PositionProvider.h"
#include <omnetpp/clistener.h>
#include <omnetpp/csimplemodule.h>
#include <vanetza/common/position_provider.hpp>

namespace artery
{

class PersonMobility;
class Runtime;

class PersonPositionProvider :
    public omnetpp::cSimpleModule, public omnetpp::cListener,
    public artery::PositionProvider, vanetza::PositionProvider
{
    public:
        // cSimpleModule
        void initialize(int stage) override;
        int numInitStages() const override;

        // cListener
        void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, omnetpp::cObject*, omnetpp::cObject*) override;

        // PositionProvider
        Position getCartesianPosition() const override;
        GeoPosition getGeodeticPosition() const override;

        // vanetza::PositionProvider
        const vanetza::PositionFix& position_fix() override { return mPositionFix; }

    private:
        void updatePosition();

        PositionFixObject mPositionFix;
        Runtime* mRuntime = nullptr;
        PersonMobility* mMobility = nullptr;
};

} // namespace artery

#endif /* ARTERY_PERSONPOSITIONPROVIDER_H_DGOSJECM */
