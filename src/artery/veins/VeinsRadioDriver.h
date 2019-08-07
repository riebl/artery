#ifndef VEINSRADIODRIVER_H_ZJ0SI5XC
#define VEINSRADIODRIVER_H_ZJ0SI5XC

#include "artery/nic/ChannelLoadSampler.h"
#include "artery/nic/RadioDriverBase.h"
#include <omnetpp/clistener.h>
#include <omnetpp/csimplemodule.h>

namespace artery
{

class VeinsRadioDriver : public RadioDriverBase, public omnetpp::cListener
{
	public:
		void initialize() override;
		void handleMessage(omnetpp::cMessage*) override;

	protected:
		void handleDataIndication(omnetpp::cMessage*);
		void handleDataRequest(omnetpp::cMessage*) override;
		void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, bool, omnetpp::cObject*) override;

	private:
		omnetpp::cModule* mHost = nullptr;
		omnetpp::cGate* mLowerLayerOut = nullptr;
		omnetpp::cGate* mLowerLayerIn = nullptr;
		omnetpp::cMessage* mChannelLoadReport = nullptr;
		omnetpp::simtime_t mChannelLoadReportInterval;
		ChannelLoadSampler mChannelLoadSampler;
};

} // namespace artery

#endif /* VEINSRADIODRIVER_H_ZJ0SI5XC */

