#ifndef INETRADIODRIVER_H_PJFDM4JW
#define INETRADIODRIVER_H_PJFDM4JW

#include <artery/nic/RadioDriverBase.h>
#include <omnetpp/clistener.h>

// forward declaration
namespace inet {
namespace ieee80211 {
class Ieee80211Mac;
} // namespace ieee80211
} // namespace inet

class InetRadioDriver : public RadioDriverBase, public omnetpp::cListener
{
	public:
		vanetza::MacAddress getMacAddress() override;
		void initialize() override;
		void handleMessage(omnetpp::cMessage*) override;

	protected:
		void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, double, omnetpp::cObject*) override;
		void handleLowerMessage(omnetpp::cMessage*);
		void handleUpperMessage(omnetpp::cMessage*) override;

	private:
		inet::ieee80211::Ieee80211Mac* mLinkLayer;
};

#endif /* INETRADIODRIVER_H_PJFDM4JW */

