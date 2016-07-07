#include "artery/traci/TraCIScenarioManagerArtery.h"
#include "artery/traci/TraCIArteryNodeManager.h"

Define_Module(TraCIScenarioManagerArtery)

const simsignal_t TraCIScenarioManagerArtery::signalUpdateStep = cComponent::registerSignal("traci.update_step");

TraCIScenarioManagerArtery::TraCIScenarioManagerArtery() :
    nodes(nullptr)
{
}

TraCIScenarioManagerArtery::~TraCIScenarioManagerArtery()
{
    delete(nodes);
}

void TraCIScenarioManagerArtery::initialize(int stage)
{
    parent_class::initialize(stage);
    if (stage == INIT_BASE) {
        nodes = new TraCIArteryNodeManager(parent_class::getNodeManager(), this);
        parent_class::setNodeManager(nodes);
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
