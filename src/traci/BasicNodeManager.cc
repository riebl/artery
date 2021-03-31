#include "traci/API.h"
#include "traci/BasicNodeManager.h"
#include "traci/CheckTimeSync.h"
#include "traci/Core.h"
#include "traci/ModuleMapper.h"
#include "traci/PersonSink.h"
#include "traci/VariableCache.h"
#include "traci/VehicleSink.h"
#include <inet/common/ModuleAccess.h>

using namespace omnetpp;

namespace traci
{
namespace
{
static const std::set<int> sPersonVariables {
    libsumo::VAR_POSITION, libsumo::VAR_SPEED, libsumo::VAR_ANGLE
};
static const std::set<int> sVehicleVariables {
    libsumo::VAR_POSITION, libsumo::VAR_SPEED, libsumo::VAR_ANGLE
};
static const std::set<int> sSimulationVariables {
    libsumo::VAR_DEPARTED_VEHICLES_IDS, libsumo::VAR_ARRIVED_VEHICLES_IDS, libsumo::VAR_TELEPORT_STARTING_VEHICLES_IDS,
    libsumo::VAR_TIME
};

class VehicleObjectImpl : public BasicNodeManager::VehicleObject
{
public:
    VehicleObjectImpl(std::shared_ptr<VehicleCache> cache) : m_cache(cache) {}

    std::shared_ptr<VehicleCache> getCache() const override { return m_cache; }
    const TraCIPosition& getPosition() const override { return m_cache->get<libsumo::VAR_POSITION>(); }
    TraCIAngle getHeading() const override { return TraCIAngle { m_cache->get<libsumo::VAR_ANGLE>() }; }
    double getSpeed() const override { return m_cache->get<libsumo::VAR_SPEED>(); }

private:
    std::shared_ptr<VehicleCache> m_cache;
};

class PersonObjectImpl : public BasicNodeManager::PersonObject
{
public:
    PersonObjectImpl(std::shared_ptr<PersonCache> cache) : m_cache(cache) {}

    std::shared_ptr<PersonCache> getCache() const override { return m_cache; }
    const TraCIPosition& getPosition() const override { return m_cache->get<libsumo::VAR_POSITION>(); }
    TraCIAngle getHeading() const override { return TraCIAngle { m_cache->get<libsumo::VAR_ANGLE>() }; }
    double getSpeed() const override { return m_cache->get<libsumo::VAR_SPEED>(); }

private:
    std::shared_ptr<PersonCache> m_cache;
};

} // namespace


Define_Module(BasicNodeManager)

const simsignal_t BasicNodeManager::addNodeSignal = cComponent::registerSignal("traci.node.add");
const simsignal_t BasicNodeManager::updateNodeSignal = cComponent::registerSignal("traci.node.update");
const simsignal_t BasicNodeManager::removeNodeSignal = cComponent::registerSignal("traci.node.remove");
const simsignal_t BasicNodeManager::addPersonSignal = cComponent::registerSignal("traci.person.add");
const simsignal_t BasicNodeManager::updatePersonSignal = cComponent::registerSignal("traci.person.update");
const simsignal_t BasicNodeManager::removePersonSignal = cComponent::registerSignal("traci.person.remove");
const simsignal_t BasicNodeManager::addVehicleSignal = cComponent::registerSignal("traci.vehicle.add");
const simsignal_t BasicNodeManager::updateVehicleSignal = cComponent::registerSignal("traci.vehicle.update");
const simsignal_t BasicNodeManager::removeVehicleSignal = cComponent::registerSignal("traci.vehicle.remove");

void BasicNodeManager::initialize()
{
    Core* core = inet::getModuleFromPar<Core>(par("coreModule"), this);
    subscribeTraCI(core);
    m_api = core->getAPI();
    m_mapper = inet::getModuleFromPar<ModuleMapper>(par("mapperModule"), this);
    m_nodeIndex = 0;
    m_person_sink_module = par("personSinkModule").stringValue();
    m_vehicle_sink_module = par("vehicleSinkModule").stringValue();
    m_subscriptions = inet::getModuleFromPar<SubscriptionManager>(par("subscriptionsModule"), this);
    m_destroy_vehicles_on_crash = par("destroyVehiclesOnCrash");
    m_ignore_persons = par("ignorePersons");

}

void BasicNodeManager::finish()
{
    unsubscribeTraCI();
    cSimpleModule::finish();
}

void BasicNodeManager::traciInit()
{
    m_boundary = Boundary { m_api->simulation.getNetBoundary() };
    m_subscriptions->subscribeSimulationVariables(sSimulationVariables);
    m_subscriptions->subscribeVehicleVariables(sVehicleVariables);

    // insert already running vehicles
    for (const std::string& id : m_api->vehicle.getIDList()) {
        addVehicle(id);
    }

    // initialize persons if enabled
    if (!m_ignore_persons) {
        m_subscriptions->subscribePersonVariables(sPersonVariables);

        // insert already running persons
        for (const std::string& id : m_api->person.getIDList()) {
            addPerson(id);
        }
    }

    // treat SUMO start time as constant offset
    m_offset = omnetpp::SimTime { m_api->simulation.getCurrentTime(), omnetpp::SIMTIME_MS };
    m_offset -= omnetpp::simTime();
}

void BasicNodeManager::traciStep()
{
    processVehicles();
    if (!m_ignore_persons) {
        processPersons();
    }
    emit(updateNodeSignal, getNumberOfNodes());
}

void BasicNodeManager::traciClose()
{
    for (unsigned i = m_nodes.size(); i > 0; --i) {
        removeNodeModule(m_nodes.begin()->first);
    }
}

void BasicNodeManager::processVehicles()
{
    auto sim_cache = m_subscriptions->getSimulationCache();
    ASSERT(checkTimeSync(*sim_cache, omnetpp::simTime() + m_offset));

    const auto& departed = sim_cache->get<libsumo::VAR_DEPARTED_VEHICLES_IDS>();
    EV_DETAIL << "TraCI: " << departed.size() << " vehicles departed" << endl;
    for (const auto& id : departed) {
        addVehicle(id);
    }

    const auto& arrived = sim_cache->get<libsumo::VAR_ARRIVED_VEHICLES_IDS>();
    EV_DETAIL << "TraCI: " << arrived.size() << " vehicles arrived" << endl;
    for (const auto& id : arrived) {
        removeVehicle(id);
    }

    if (m_destroy_vehicles_on_crash) {
        const auto& teleport = sim_cache->get<libsumo::VAR_TELEPORT_STARTING_VEHICLES_IDS>();
        for (const auto& id : teleport) {
            EV_DETAIL << "TraCI: " << id << " got teleported and is removed!" << endl;
            removeVehicle(id);
        }
    }

    for (auto& vehicle : m_vehicles) {
        const std::string& id = vehicle.first;
        VehicleSink* sink = vehicle.second;
        updateVehicle(id, sink);
    }
}

void BasicNodeManager::addVehicle(const std::string& id)
{
    NodeInitializer init = [this, &id](cModule* module) {
        VehicleSink* vehicle = getVehicleSink(module);
        auto& traci = m_api->vehicle;
        vehicle->initializeSink(m_api, m_subscriptions->getVehicleCache(id), m_boundary);
        vehicle->initializeVehicle(traci.getPosition(id), TraCIAngle { traci.getAngle(id) }, traci.getSpeed(id));
        m_vehicles[id] = vehicle;
    };

    emit(addVehicleSignal, id.c_str());
    cModuleType* type = m_mapper->vehicle(*this, id);
    if (type != nullptr) {
        addNodeModule(id, type, init);
    } else {
        m_vehicles[id] = nullptr;
    }
}

void BasicNodeManager::removeVehicle(const std::string& id)
{
    emit(removeVehicleSignal, id.c_str());
    removeNodeModule(id);
    m_vehicles.erase(id);
}

void BasicNodeManager::updateVehicle(const std::string& id, VehicleSink* sink)
{
    auto vehicle = m_subscriptions->getVehicleCache(id);
    VehicleObjectImpl update(vehicle);
    emit(updateVehicleSignal, id.c_str(), &update);
    if (sink) {
        sink->updateVehicle(vehicle->get<libsumo::VAR_POSITION>(),
                TraCIAngle { vehicle->get<libsumo::VAR_ANGLE>() },
                vehicle->get<libsumo::VAR_SPEED>());
    }
}

void BasicNodeManager::processPersons()
{
    auto sim_cache = m_subscriptions->getSimulationCache();

    const auto& departed = sim_cache->get<libsumo::VAR_DEPARTED_PERSONS_IDS>();
    EV_DETAIL << "TraCI: " << departed.size() << " persons departed" << endl;
    for (const auto& id : departed) {
        addPerson(id);
    }

    const auto& arrived = sim_cache->get<libsumo::VAR_ARRIVED_PERSONS_IDS>();
    EV_DETAIL << "TraCI: " << arrived.size() << " persons arrived" << endl;
    for (const auto& id : arrived) {
        removePerson(id);
    }

    for (auto& person : m_persons) {
        const std::string& id = person.first;
        PersonSink* sink = person.second;
        updatePerson(id, sink);
    }
}

void BasicNodeManager::addPerson(const std::string& id)
{
    NodeInitializer init = [this, &id](cModule* module) {
        PersonSink* person = getPersonSink(module);
        auto& traci = m_api->person;
        person->initializeSink(m_api, m_subscriptions->getPersonCache(id), m_boundary);
        person->initializePerson(traci.getPosition(id), TraCIAngle { traci.getAngle(id) }, traci.getSpeed(id));
        m_persons[id] = person;
    };

    emit(addPersonSignal, id.c_str());
    cModuleType* type = m_mapper->person(*this, id);
    if (type != nullptr) {
        addNodeModule(id, type, init);
    } else {
        m_persons[id] = nullptr;
    }
}

void BasicNodeManager::removePerson(const std::string& id)
{
    emit(removePersonSignal, id.c_str());
    removeNodeModule(id);
    m_persons.erase(id);
}

void BasicNodeManager::updatePerson(const std::string& id, PersonSink* sink)
{
    auto person = m_subscriptions->getPersonCache(id);
    PersonObjectImpl update(person);
    emit(updatePersonSignal, id.c_str(), &update);
    if (sink) {
        sink->updatePerson(person->get<libsumo::VAR_POSITION>(),
                TraCIAngle { person->get<libsumo::VAR_ANGLE>() },
                person->get<libsumo::VAR_SPEED>());
    }
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
        EV_DEBUG << "Node with id " << id << " does not exist, no removal\n";
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
    cModule* module = node->getModuleByPath(m_vehicle_sink_module.c_str());
    if (!module) {
        throw cRuntimeError("No module found at %s relative to %s", m_vehicle_sink_module.c_str(), node->getFullPath().c_str());
    }

    auto* mobility = dynamic_cast<VehicleSink*>(module);
    if (!mobility) {
        throw cRuntimeError("Module %s is not a VehicleSink", module->getFullPath().c_str());
    }

    return mobility;
}

VehicleSink* BasicNodeManager::getVehicleSink(const std::string& id)
{
    auto found = m_vehicles.find(id);
    return found != m_vehicles.end() ? found->second : nullptr;
}

PersonSink* BasicNodeManager::getPersonSink(cModule* node)
{
    ASSERT(node);
    cModule* module = node->getModuleByPath(m_person_sink_module.c_str());
    if (!module) {
        throw cRuntimeError("No module found at %s relative to %s", m_person_sink_module.c_str(), node->getFullPath().c_str());
    }

    auto* mobility = dynamic_cast<PersonSink*>(module);
    if (!mobility) {
        throw cRuntimeError("Module %s is not a PersonSink", module->getFullPath().c_str());
    }

    return mobility;
}

PersonSink* BasicNodeManager::getPersonSink(const std::string& id)
{
    auto found = m_persons.find(id);
    return found != m_persons.end() ? found->second : nullptr;
}

} // namespace traci
