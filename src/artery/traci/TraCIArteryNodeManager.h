#ifndef ARTERY_TRACIARTERYNODEMANAGER_H_ZLFJ1KXJ
#define ARTERY_TRACIARTERYNODEMANAGER_H_ZLFJ1KXJ

#include "veins/modules/mobility/traci/TraCINodeManager.h"

class TraCIArteryNodeManager : public Veins::TraCINodeManager
{
public:
    TraCIArteryNodeManager(Veins::TraCINodeManager*, cModule* emitter);

    void add(const std::string& nodeId, const NodeData&, const std::string& vehicleType, simtime_t) override;
    void remove(const std::string& nodeId) override;
    void update(const std::string& nodeId, const NodeData&) override;
    cModule* get(const std::string& nodeId) override;
    void finish() override;
    size_t size() override;

    static const simsignal_t signalAddNode;
    static const simsignal_t signalRemoveNode;
    static const simsignal_t signalUpdateNode;

private:
    Veins::TraCINodeManager* nodeManager;
    cModule* emitter;
};

#endif /* ARTERY_TRACIARTERYNODEMANAGER_H_ZLFJ1KXJ */

