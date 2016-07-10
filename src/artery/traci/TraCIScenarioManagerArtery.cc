#include "artery/traci/TraCIArteryNodeManager.h"
#include "artery/traci/TraCISimpleNodeManager.h"
#include "artery/traci/TraCIScenarioManagerArtery.h"
#include "veins/modules/mobility/traci/TraCIScenarioManagerLaunchd.h"
#include <cassert>

Define_Module(TraCIScenarioManagerArtery)

const simsignal_t TraCIScenarioManagerArtery::signalUpdateStep = cComponent::registerSignal("traci.update_step");

TraCIScenarioManagerArtery::TraCIScenarioManagerArtery() :
    m_launch_config(nullptr), m_seed(-1)
{
}

TraCIScenarioManagerArtery::~TraCIScenarioManagerArtery()
{
}

void TraCIScenarioManagerArtery::initialize(int stage)
{
    parent_class::initialize(stage);
    if (stage == INIT_BASE) {
        const std::string moduleTypes = par("moduleType").stdstringValue();
        const std::string moduleNames = par("moduleName").stdstringValue();
        const std::string moduleDisplayStrings = par("moduleDisplayString").stdstringValue();
        m_mapper.parseConfig(moduleTypes, moduleNames, moduleDisplayStrings);

        std::unique_ptr<TraCISimpleNodeManager> nodes_base { new TraCISimpleNodeManager(m_mapper, getParentModule()) };
        nodes_base->setMobilityModule(par("mobilityNodeModule"));
        m_nodes_base = std::move(nodes_base);
        m_nodes.reset(new TraCIArteryNodeManager(m_nodes_base.get(), this));
        parent_class::setNodeManager(m_nodes.get());

        m_launch_config = par("launchConfig").xmlValue();
        m_seed = par("seed");
        Veins::TraCIScenarioManagerLaunchd::prepareSeed(m_seed);
        Veins::TraCIScenarioManagerLaunchd::prepareLaunchConfig(m_launch_config, m_seed);
    }
}

void TraCIScenarioManagerArtery::handleSelfMsg(cMessage* msg)
{
    bool update = (msg == executeOneTimestepTrigger);
    parent_class::handleSelfMsg(msg);
    if (update) {
        emit(signalUpdateStep, updateInterval);
    }
}

void TraCIScenarioManagerArtery::init_traci()
{
    assert(m_launch_config);
    Veins::TraCIScenarioManagerLaunchd::checkLaunchdCompatibility(*getCommandInterface());
    Veins::TraCIScenarioManagerLaunchd::sendFileCommand(*connection, m_launch_config);
    parent_class::init_traci();
}
