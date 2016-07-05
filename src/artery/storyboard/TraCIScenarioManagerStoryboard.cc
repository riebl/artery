#include "artery/storyboard/TraCIScenarioManagerStoryboard.h"
#include "artery/traci/TraCIArteryNodeManager.h"

Define_Module(TraCIScenarioManagerStoryboard)

const simsignal_t TraCIScenarioManagerStoryboard::signalUpdateStep = cComponent::registerSignal("traci.update_step");

TraCIScenarioManagerStoryboard::TraCIScenarioManagerStoryboard() :
    nodes(nullptr)
{
}

TraCIScenarioManagerStoryboard::~TraCIScenarioManagerStoryboard()
{
    delete(nodes);
}

void TraCIScenarioManagerStoryboard::initialize(int stage)
{
    parent_class::initialize(stage);
    if (stage == INIT_BASE) {
        nodes = new TraCIArteryNodeManager(parent_class::getNodeManager(), this);
        parent_class::setNodeManager(nodes);
    }
}

void TraCIScenarioManagerStoryboard::handleSelfMsg(cMessage* msg)
{
    bool update = (msg == executeOneTimestepTrigger);
    parent_class::handleSelfMsg(msg);
    if (update) {
        emit(signalUpdateStep, updateInterval);
    }
}
