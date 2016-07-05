#include "artery/traci/TraCIArteryNodeManager.h"

const simsignal_t TraCIArteryNodeManager::signalAddNode = cComponent::registerSignal("traci.add_node");
const simsignal_t TraCIArteryNodeManager::signalRemoveNode = cComponent::registerSignal("traci.remove_node");
const simsignal_t TraCIArteryNodeManager::signalUpdateNode = cComponent::registerSignal("traci.update_node");

TraCIArteryNodeManager::TraCIArteryNodeManager(Veins::TraCINodeManager* nodeManager_, cModule* emitter_) :
    nodeManager(nodeManager_), emitter(emitter_)
{
}

void TraCIArteryNodeManager::add(const std::string& nodeId, const NodeData& data, const std::string& vehicleType, simtime_t start)
{
    nodeManager->add(nodeId, data, vehicleType, start);
    emitter->emit(signalAddNode, nodeId.c_str());
}

void TraCIArteryNodeManager::remove(const std::string& nodeId)
{
    emitter->emit(signalRemoveNode, nodeId.c_str());
    nodeManager->remove(nodeId);
}

void TraCIArteryNodeManager::update(const std::string& nodeId, const NodeData& data)
{
    nodeManager->update(nodeId, data);
    emitter->emit(signalUpdateNode, nodeId.c_str());
}

cModule* TraCIArteryNodeManager::get(const std::string& nodeId)
{
    return nodeManager->get(nodeId);
}

void TraCIArteryNodeManager::finish()
{
    nodeManager->finish();
}

size_t TraCIArteryNodeManager::size()
{
    return nodeManager->size();
}
