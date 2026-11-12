#pragma once

#include <omnetpp/cmessage.h>
#include <omnetpp/csimplemodule.h>
#include <omnetpp/simtime.h>
#include <traci/API.h>
#include <traci/launchers/Launcher.h>

#include <memory>

namespace artery
{

namespace traci
{

class SubscriptionManager;

class Core : public omnetpp::cSimpleModule
{
public:
    Core();
    virtual ~Core();

    void initialize() override;
    void finish() override;
    void handleMessage(omnetpp::cMessage*) override;
    std::shared_ptr<API> getAPI();

protected:
    virtual void checkVersion();
    virtual void syncTime();

private:
    omnetpp::cMessage* m_connectEvent;
    omnetpp::cMessage* m_updateEvent;
    omnetpp::SimTime m_updateInterval;

    ILauncher* m_launcher;
    std::shared_ptr<API> m_traci;
    bool m_stopping;
    SubscriptionManager* m_subscriptions;
};

}  // namespace traci

}  // namespace artery
