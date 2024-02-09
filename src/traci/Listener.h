#ifndef LISTENER_H_ISXJK7OC
#define LISTENER_H_ISXJK7OC

#include <omnetpp/clistener.h>

namespace traci
{
/**
 * Do-nothing implementation of omnetpp::cListener for the Traci-API.
 * Extend this class and override its functions in order to receive Traci-Signal conveniently.
 */
class Listener : public omnetpp::cListener
{
public:
    Listener();
    virtual ~Listener() = default;

    /**
     * Call this function with your component to subscribe to Traci-Signal.
     * \example Listener::subscribeTraCI(getSystemModule());
     */
    void subscribeTraCI(omnetpp::cComponent*);
    void unsubscribeTraCI();
    void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, const omnetpp::SimTime&, omnetpp::cObject*) override;

private:
    /**
     * These functions are called as soon as a signal is received. Override these function in order to get notified.
     * @{
     */
    virtual void traciInit();
    virtual void traciStep();
    virtual void traciClose();
    /** @} */

    omnetpp::cComponent* m_publisher;
};

} // namespace traci

#endif /* LISTENER_H_ISXJK7OC */

