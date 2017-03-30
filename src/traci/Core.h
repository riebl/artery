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

class Core : public omnetpp::cSimpleModule
{
public:
    Core();

    void initialize() override;
    void finish() override;
    void handleMessage(omnetpp::cMessage*) override;
    LiteAPI& getLiteAPI();

protected:
    virtual void checkVersion();

private:
    omnetpp::cMessage* m_connectEvent;
    omnetpp::cMessage* m_updateEvent;
    omnetpp::SimTime m_updateInterval;

    Launcher* m_launcher;
    std::unique_ptr<API> m_traci;
    std::unique_ptr<LiteAPI> m_lite;
    bool m_stopping;
};

} // namespace traci

#endif /* CORE_H_HPQGM1MF */

