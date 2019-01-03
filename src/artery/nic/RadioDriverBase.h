#ifndef RADIODRIVERBASE_H_IHIKLETJ
#define RADIODRIVERBASE_H_IHIKLETJ

#include <omnetpp/ccomponent.h>
#include <omnetpp/cmessage.h>
#include <omnetpp/csimplemodule.h>

namespace artery
{

struct RadioDriverProperties;

class RadioDriverBase : public omnetpp::cSimpleModule
{
    public:
        static const omnetpp::simsignal_t ChannelLoadSignal;

        virtual void initialize() override;
        virtual void handleMessage(omnetpp::cMessage*) override;

    protected:
        void indicateProperties(RadioDriverProperties*);
        void indicateData(omnetpp::cMessage*);
        bool isDataRequest(omnetpp::cMessage*);
        virtual void handleDataRequest(omnetpp::cMessage*) = 0;

    private:
        omnetpp::cGate* mUpperLayerIn;
        omnetpp::cGate* mUpperLayerOut;
        omnetpp::cGate* mPropertiesOut;
};

} // namespace artery

#endif /* RADIODRIVERBASE_H_IHIKLETJ */

