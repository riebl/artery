#ifndef VEINSRADIODRIVER_H_ZJ0SI5XC
#define VEINSRADIODRIVER_H_ZJ0SI5XC

#include <artery/mac/ChannelLoadMeasurements.h>
#include <artery/nic/RadioDriverBase.h>
#include <omnetpp/clistener.h>
#include <omnetpp/csimplemodule.h>

class VeinsRadioDriver : public RadioDriverBase, public omnetpp::cListener
{
	public:
		vanetza::MacAddress getMacAddress() override;
		void initialize() override;
		void handleMessage(omnetpp::cMessage*) override;

	protected:
		void handleLowerMessage(omnetpp::cMessage*);
		void handleUpperMessage(omnetpp::cMessage*) override;
		void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, bool, omnetpp::cObject*) override;

	private:
		omnetpp::cModule* mHost = nullptr;
		omnetpp::cGate* mLowerLayerOut = nullptr;
		omnetpp::cGate* mLowerLayerIn = nullptr;
		omnetpp::cMessage* mChannelLoadReport = nullptr;
		omnetpp::simtime_t mChannelLoadReportInterval;
		ChannelLoadMeasurements mChannelLoadMeasurements;
};

#endif /* VEINSRADIODRIVER_H_ZJ0SI5XC */

