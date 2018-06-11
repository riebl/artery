#ifndef CORE_H_HPQGM1MF
#define CORE_H_HPQGM1MF

#include <omnetpp/cmessage.h>
#include <omnetpp/csimplemodule.h>
#include <omnetpp/simtime.h>
#include <memory>

namespace traci
{

class API;
class Launcher;
class LiteAPI;
class SubscriptionManager;

class Core : public omnetpp::cSimpleModule
{
public:
    Core();
    virtual ~Core();

    void initialize() override;
    void finish() override;
    void handleMessage(omnetpp::cMessage*) override;
    LiteAPI& getLiteAPI();

protected:
    virtual void checkVersion();
    virtual void syncTime();

private:
    omnetpp::cMessage* m_connectEvent;
    omnetpp::cMessage* m_updateEvent;
    omnetpp::SimTime m_updateInterval;

    Launcher* m_launcher;
    std::unique_ptr<API> m_traci;
    std::unique_ptr<LiteAPI> m_lite;
    bool m_stopping;
    SubscriptionManager* m_subscriptions;
};

} // namespace traci

#endif /* CORE_H_HPQGM1MF */

