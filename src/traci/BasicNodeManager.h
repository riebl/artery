#ifndef BASICNODEMANAGER_H_XL6ISC2V
#define BASICNODEMANAGER_H_XL6ISC2V

#include "traci/Boundary.h"
#include "traci/NodeManager.h"
#include "traci/Listener.h"
#include <omnetpp/ccomponent.h>
#include <omnetpp/csimplemodule.h>
#include <functional>
#include <map>
#include <string>

namespace traci
{

class LiteAPI;
class ModuleMapper;
class VehicleSink;

class BasicNodeManager : public NodeManager, public Listener, public omnetpp::cSimpleModule
{
public:
    static const omnetpp::simsignal_t addNodeSignal;
    static const omnetpp::simsignal_t updateNodeSignal;
    static const omnetpp::simsignal_t removeNodeSignal;

    LiteAPI* getLiteAPI() override { return m_api; }
    std::size_t getNumberOfNodes() const override;

protected:
    using NodeInitializer = std::function<void(omnetpp::cModule*)>;

    void initialize() override;
    void finish() override;

    virtual void addVehicle(const std::string&);
    virtual void removeVehicle(const std::string&);
    virtual void updateVehicle(const std::string&);
    virtual void updateVehicle(const std::string&, VehicleSink*);
    virtual omnetpp::cModule* addNodeModule(const std::string&, omnetpp::cModuleType*, NodeInitializer&);
    virtual void removeNodeModule(const std::string&);
    virtual omnetpp::cModule* getNodeModule(const std::string&);
    virtual VehicleSink* getVehicleSink(omnetpp::cModule*);
    virtual void subscribeVehicle(const std::string&);
    virtual void unsubscribeVehicle(const std::string&);
    virtual const std::vector<int>& vehicleSubscriptionVariables(const std::string&);

private:
    void traciInit() override;
    void traciStep() override;
    void traciClose() override;

    LiteAPI* m_api;
    ModuleMapper* m_mapper;
    TraCIBoundary m_boundary;
    std::map<std::string, omnetpp::cModule*> m_nodes;
    std::map<std::string, VehicleSink*> m_vehicles;
    unsigned m_nodeIndex;
    std::string m_vehicleSinkModule;
};

} // namespace traci

#endif /* BASICNODEMANAGER_H_XL6ISC2V */

