#ifndef RADIODRIVERBASE_H_IHIKLETJ
#define RADIODRIVERBASE_H_IHIKLETJ

#include <omnetpp/ccomponent.h>
#include <omnetpp/cmessage.h>
#include <omnetpp/csimplemodule.h>
#include <vanetza/net/mac_address.hpp>

namespace artery
{

class RadioDriverBase : public omnetpp::cSimpleModule
{
	public:
		virtual vanetza::MacAddress getMacAddress() = 0;
		virtual void initialize() override;
		virtual void handleMessage(omnetpp::cMessage*) override;

		static const omnetpp::simsignal_t ChannelLoadSignal;

	protected:
		void indicatePacket(omnetpp::cMessage*);
		bool isMiddlewareRequest(omnetpp::cMessage*);
		virtual void handleUpperMessage(omnetpp::cMessage*) = 0;

	private:
		omnetpp::cGate* mUpperLayerIn;
		omnetpp::cGate* mUpperLayerOut;
};

} // namespace artery

#endif /* RADIODRIVERBASE_H_IHIKLETJ */

