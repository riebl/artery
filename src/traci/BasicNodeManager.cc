#include "traci/BasicNodeManager.h"
#include "traci/Core.h"
#include "traci/LiteAPI.h"
#include "traci/ModuleMapper.h"
#include "traci/VariableCache.h"
#include "traci/VehicleSink.h"
#include <inet/common/ModuleAccess.h>

using namespace omnetpp;

namespace traci
{
namespace
{
static const std::set<int> sVehicleVariables {
    VAR_POSITION, VAR_SPEED, VAR_ANGLE
};
static const std::set<int> sSimulationVariables {
    VAR_DEPARTED_VEHICLES_IDS, VAR_ARRIVED_VEHICLES_IDS, VAR_TIME_STEP
};
} // namespace

Define_Module(BasicNodeManager)

const simsignal_t BasicNodeManager::addNodeSignal = cComponent::registerSignal("traci.node.add");
const simsignal_t BasicNodeManager::updateNodeSignal = cComponent::registerSignal("traci.node.update");
const simsignal_t BasicNodeManager::removeNodeSignal = cComponent::registerSignal("traci.node.remove");

void BasicNodeManager::initialize()
{
    Core* core = inet::getModuleFromPar<Core>(par("coreModule"), this);
    subscribeTraCI(core);
    m_api = &core->getLiteAPI();
    m_mapper = inet::getModuleFromPar<ModuleMapper>(par("mapperModule"), this);
    m_nodeIndex = 0;
    m_vehicleSinkModule = par("vehicleSinkModule").stringValue();
    m_subscriptions = inet::getModuleFromPar<SubscriptionManager>(par("subscriptionsModule"), this);
}

void BasicNodeManager::finish()
{
    m_api = nullptr;
    unsubscribeTraCI();
    cSimpleModule::finish();
}

void BasicNodeManager::traciInit()
{
    using namespace traci::constants;
    m_boundary = m_api->simulation().getNetBoundary();
    m_subscriptions->subscribeSimulationVariables(sSimulationVariables);
    m_subscriptions->subscribeVehicleVariables(sVehicleVariables);

    // insert already running vehicles
    for (const std::string& id : m_api->vehicle().getIDList()) {
        addVehicle(id);
    }
}

void BasicNodeManager::traciStep()
{
    auto sim_cache = m_subscriptions->getSimulationCache();
    ASSERT(time_cast(sim_cache->get<VAR_TIME_STEP>()) == simTime());

    const auto& departed = sim_cache->get<VAR_DEPARTED_VEHICLES_IDS>();
    EV_DETAIL << "TraCI: " << departed.size() << " vehicles departed" << endl;
    for (const auto& id : departed) {
        addVehicle(id);
    }

    const auto& arrived = sim_cache->get<VAR_ARRIVED_VEHICLES_IDS>();
    EV_DETAIL << "TraCI: " << arrived.size() << " vehicles arrived" << endl;
    for (const auto& id : arrived) {
        removeVehicle(id);
    }

    for (auto& vehicle : m_vehicles) {
        updateVehicle(vehicle.first, vehicle.second);
    }

    for (auto& node : m_nodes) {
        emit(updateNodeSignal, node.first.c_str(), node.second);
    }
}

void BasicNodeManager::traciClose()
{
    for (unsigned i = m_nodes.size(); i > 0; --i) {
        removeNodeModule(m_nodes.begin()->first);
    }
}

void BasicNodeManager::addVehicle(const std::string& id)
{
    NodeInitializer init = [this, &id](cModule* module) {
        VehicleSink* vehicle = getVehicleSink(module);
        auto& traci = m_api->vehicle();
        vehicle->initializeSink(m_api, id, m_boundary, m_subscriptions->getVehicleCache(id));
        vehicle->initializeVehicle(traci.getPosition(id), TraCIAngle { traci.getAngle(id) }, traci.getSpeed(id));
        m_vehicles[id] = vehicle;
    };

    cModuleType* type = m_mapper->vehicle(*this, id);
    if (type != nullptr) {
        addNodeModule(id, type, init);
    }
}

void BasicNodeManager::removeVehicle(const std::string& id)
{
    removeNodeModule(id);
    m_vehicles.erase(id);
}

void BasicNodeManager::updateVehicle(const std::string& id)
{
    updateVehicle(id, m_vehicles.at(id));
}

void BasicNodeManager::updateVehicle(const std::string& id, VehicleSink* sink)
{
    ASSERT(sink);
    auto vehicle = m_subscriptions->getVehicleCache(id);
    sink->updateVehicle(vehicle->get<VAR_POSITION>(),
            TraCIAngle { vehicle->get<VAR_ANGLE>() },
            vehicle->get<VAR_SPEED>() );
}

cModule* BasicNodeManager::createModule(const std::string&, cModuleType* type)
{
    cModule* module = type->create("node", getSystemModule(), m_nodeIndex, m_nodeIndex);
    ++m_nodeIndex;
    return module;
}

cModule* BasicNodeManager::addNodeModule(const std::string& id, cModuleType* type, NodeInitializer& init)
{
    cModule* module = createModule(id, type);
    module->finalizeParameters();
    module->buildInside();
    m_nodes[id] = module;
    init(module);
    module->scheduleStart(simTime());
    module->callInitialize();
    emit(addNodeSignal, id.c_str(), module);

    return module;
}

void BasicNodeManager::removeNodeModule(const std::string& id)
{
    cModule* module = getNodeModule(id);
    if (module) {
        emit(removeNodeSignal, id.c_str(), module);
        module->callFinish();
        module->deleteModule();
        m_nodes.erase(id);
    } else {
        throw cRuntimeError("Node with id %s does not exist", id.c_str());
    }
}

cModule* BasicNodeManager::getNodeModule(const std::string& id)
{
    auto found = m_nodes.find(id);
    return found != m_nodes.end() ? found->second : nullptr;
}

std::size_t BasicNodeManager::getNumberOfNodes() const
{
    return m_nodes.size();
}

VehicleSink* BasicNodeManager::getVehicleSink(cModule* node)
{
    ASSERT(node);
    cModule* module = node->getModuleByPath(m_vehicleSinkModule.c_str());
    if (!module) {
        throw cRuntimeError("No module found at %s relative to %s", m_vehicleSinkModule.c_str(), node->getFullPath().c_str());
    }

    auto* mobility = dynamic_cast<VehicleSink*>(module);
    if (!mobility) {
        throw cRuntimeError("Module %s is not a VehicleSink", module->getFullPath().c_str());
    }

    return mobility;
}

} // namespace traci
