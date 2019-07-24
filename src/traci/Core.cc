#include "traci/Core.h"
#include "traci/Launcher.h"
#include "traci/LiteAPI.h"
#include "traci/API.h"
#include "traci/SubscriptionManager.h"
#include <inet/common/ModuleAccess.h>

Define_Module(traci::Core)

using namespace omnetpp;
using std::endl;

namespace
{
const simsignal_t initSignal = cComponent::registerSignal("traci.init");
const simsignal_t stepSignal = cComponent::registerSignal("traci.step");
const simsignal_t closeSignal = cComponent::registerSignal("traci.close");
}

namespace traci
{

Core::Core() : m_traci(new API()), m_lite(new LiteAPI(*m_traci)), m_subscriptions(nullptr)
{
}

Core::~Core()
{
    cancelAndDelete(m_connectEvent);
    cancelAndDelete(m_updateEvent);
}

void Core::initialize()
{
    m_connectEvent = new cMessage("connect TraCI");
    m_updateEvent = new cMessage("TraCI step");
    cModule* manager = getParentModule();
    m_launcher = inet::getModuleFromPar<Launcher>(par("launcherModule"), manager);
    m_stopping = par("selfStopping");
    scheduleAt(par("startTime"), m_connectEvent);
    m_subscriptions = inet::getModuleFromPar<SubscriptionManager>(par("subscriptionsModule"), manager, false);
}

void Core::finish()
{
    emit(closeSignal, simTime());
    if (!m_connectEvent->isScheduled()) {
        m_traci->close();
    }
}

void Core::handleMessage(cMessage* msg)
{
    if (msg == m_updateEvent) {
        m_traci->simulationStep();
        if (m_subscriptions) {
            m_subscriptions->step();
        }
        emit(stepSignal, simTime());

        if (!m_stopping || m_traci->simulation.getMinExpectedNumber() > 0) {
            scheduleAt(simTime() + m_updateInterval, m_updateEvent);
        }
    } else if (msg == m_connectEvent) {
        m_traci->connect(m_launcher->launch());
        checkVersion();
        syncTime();
        emit(initSignal, simTime());
        m_updateInterval = Time { m_traci->simulation.getDeltaT() };
        scheduleAt(simTime() + m_updateInterval, m_updateEvent);
    }
}

void Core::checkVersion()
{
    int expected = par("version");
    if (expected == 0) {
        expected = libsumo::TRACI_VERSION;
        EV_INFO << "Defaulting expected TraCI API level to client API version " << expected << endl;
    }

    const auto actual = m_traci->getVersion();
    EV_INFO << "TraCI server is " << actual.second << " with API level " << actual.first << endl;

    if (actual.first < 18) {
        EV_FATAL << "Reported TraCI server version is incompatible with client API" << endl;
        throw cRuntimeError("Version of TraCI server is too old (required: 18, provided: %i), please update SUMO!", actual.first);
    } else if (expected < 0) {
        EV_DEBUG << "No specific TraCI server version requested, accepting connection..." << endl;
    } else if (expected != actual.first) {
        EV_FATAL << "Reported TraCI server version does not match expected version " << expected << endl;
        throw cRuntimeError("TraCI server version mismatch (expected: %i, actual: %i)", expected, actual.first);
    }
}

void Core::syncTime()
{
    const SimTime now = simTime();
    if (!now.isZero()) {
        m_traci->simulationStep(now.dbl());
    }
}

LiteAPI& Core::getLiteAPI()
{
    return *m_lite;
}

} // namespace traci
