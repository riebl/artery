#ifndef INETRADIODRIVER_H_PJFDM4JW
#define INETRADIODRIVER_H_PJFDM4JW

#include <artery/nic/RadioDriverBase.h>
#include <omnetpp/clistener.h>

// forward declaration
namespace inet {
namespace ieee80211 { class Ieee80211Mac; }
namespace physicallayer { class Ieee80211Radio; }
} // namespace inet

namespace artery
{

class InetRadioDriver : public RadioDriverBase, public omnetpp::cListener
{
    public:
        int numInitStages() const override;
        void initialize(int stage) override;
        void handleMessage(omnetpp::cMessage*) override;

    protected:
        void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, double, omnetpp::cObject*) override;
        void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, long, omnetpp::cObject*) override;
        void handleDataIndication(omnetpp::cMessage*);
        void handleDataRequest(omnetpp::cMessage*) override;

    private:
        inet::ieee80211::Ieee80211Mac* mLinkLayer = nullptr;
        inet::physicallayer::Ieee80211Radio* mRadio = nullptr;
        int mChannelNumber = 0;
};

} // namespace artery

#endif /* INETRADIODRIVER_H_PJFDM4JW */

