#ifndef VEINSRADIODRIVER_H_ZJ0SI5XC
#define VEINSRADIODRIVER_H_ZJ0SI5XC

#include <artery/nic/RadioDriverBase.h>
#include <omnetpp/csimplemodule.h>

class VeinsRadioDriver : public RadioDriverBase
{
	public:
		vanetza::MacAddress getMacAddress() override;
		void initialize() override;
		void handleMessage(omnetpp::cMessage*) override;

	protected:
		void handleLowerMessage(omnetpp::cMessage*);
		void handleLowerControl(omnetpp::cMessage*);
		void handleUpperMessage(omnetpp::cMessage*) override;

	private:
		omnetpp::cGate* mLowerLayerOut;
		omnetpp::cGate* mLowerLayerIn;
		omnetpp::cGate* mLowerControlIn;
};

#endif /* VEINSRADIODRIVER_H_ZJ0SI5XC */

