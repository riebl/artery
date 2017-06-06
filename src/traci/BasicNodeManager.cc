#include "traci/BasicNodeManager.h"
#include "traci/Core.h"
#include "traci/LiteAPI.h"
#include "traci/ModuleMapper.h"
#include "traci/VehicleSink.h"
#include <inet/common/ModuleAccess.h>

using namespace omnetpp;

namespace traci
{

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

    static const std::vector<int> vars {
        VAR_DEPARTED_VEHICLES_IDS,
        VAR_ARRIVED_VEHICLES_IDS,
        VAR_TIME_STEP
    };
    m_api->simulation().subscribe(CMD_SUBSCRIBE_SIM_VARIABLE, "", TraCITime::min(), TraCITime::max(), vars);
    m_boundary = m_api->simulation().getNetBoundary();
}

void BasicNodeManager::traciStep()
{
    auto results = m_api->simulation().getSubscriptionResults("");
    ASSERT(time_cast(results[VAR_TIME_STEP].scalar) == simTime());

    const std::vector<std::string> departed = results[VAR_DEPARTED_VEHICLES_IDS].stringList;
    EV_DETAIL << "TraCI: " << departed.size() << " vehicles departed" << endl;
    for (const auto& id : departed) {
        addVehicle(id);
    }

    const std::vector<std::string> arrived = results[VAR_ARRIVED_VEHICLES_IDS].stringList;
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
    for (auto& node : m_nodes) {
        removeNodeModule(node.first);
    }
}

void BasicNodeManager::addVehicle(const std::string& id)
{
    NodeInitializer init = [this, &id](cModule* module) {
        VehicleSink* vehicle = getVehicleSink(module);
        vehicle->initializeVehicle(m_api, id, m_boundary);
        m_vehicles[id] = vehicle;
    };

    subscribeVehicle(id);
    cModuleType* type = m_mapper->vehicle(*this, id);
    if (type != nullptr) {
        addNodeModule(id, type, init);
    }
}

void BasicNodeManager::removeVehicle(const std::string& id)
{
    unsubscribeVehicle(id);
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
    auto vehicle = m_api->simulation().getSubscriptionResults(id);
    if (vehicle.empty()) {
        throw cRuntimeError("Vehicle subscription data are empty for %s", id.c_str());
    }
    sink->updateVehicle(vehicle[VAR_POSITION].position,
            TraCIAngle {vehicle[VAR_ANGLE].scalar},
            vehicle[VAR_SPEED].scalar);
}

cModule* BasicNodeManager::addNodeModule(const std::string& id, cModuleType* type, NodeInitializer& init)
{
    ++m_nodeIndex;
    cModule* module = type->create("node", getSystemModule(), m_nodeIndex, m_nodeIndex);
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

void BasicNodeManager::subscribeVehicle(const std::string& id)
{
    using namespace traci::constants;
    const std::vector<int>& vars = vehicleSubscriptionVariables(id);
    m_api->simulation().subscribe(CMD_SUBSCRIBE_VEHICLE_VARIABLE, id, TraCITime::min(), TraCITime::max(), vars);
}

void BasicNodeManager::unsubscribeVehicle(const std::string& id)
{
    using namespace traci::constants;
    static const std::vector<int> vars {};
    m_api->simulation().subscribe(CMD_SUBSCRIBE_VEHICLE_VARIABLE, id, TraCITime::min(), TraCITime::max(), vars);
}

const std::vector<int>& BasicNodeManager::vehicleSubscriptionVariables(const std::string& /* unused_id */)
{
    static const std::vector<int> vars {
        VAR_POSITION,
        VAR_SPEED,
        VAR_ANGLE
    };
    return vars;
}

} // namespace traci
