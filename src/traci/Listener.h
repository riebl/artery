#ifndef LISTENER_H_ISXJK7OC
#define LISTENER_H_ISXJK7OC

#include <omnetpp/clistener.h>

namespace traci
{

class Listener : public omnetpp::cListener
{
public:
    Listener();
    virtual ~Listener() = default;
    void subscribeTraCI(omnetpp::cComponent*);
    void unsubscribeTraCI();
    void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, const omnetpp::SimTime&, omnetpp::cObject*) override;

private:
    virtual void traciInit();
    virtual void traciStep();
    virtual void traciClose();

    omnetpp::cComponent* m_publisher;
};

} // namespace traci

#endif /* LISTENER_H_ISXJK7OC */

