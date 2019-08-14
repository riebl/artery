#include "traci/BasicNodeManager.h"
#include "traci/CheckTimeSync.h"
#include "traci/Core.h"
#include "traci/LiteAPI.h"
#include "traci/ModuleMapper.h"
#include "traci/VariableCache.h"
#include "traci/VehicleSink.h"
#include <inet/common/ModuleAccess.h>
#include "artery/inet/RSUMobility.h"
#include <artery/inet/AntennaMobility.h>

using namespace omnetpp;

namespace traci
{
namespace
{
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

} // namespace


Define_Module(BasicNodeManager)

const simsignal_t BasicNodeManager::addNodeSignal = cComponent::registerSignal("traci.node.add");
const simsignal_t BasicNodeManager::updateNodeSignal = cComponent::registerSignal("traci.node.update");
const simsignal_t BasicNodeManager::removeNodeSignal = cComponent::registerSignal("traci.node.remove");
const simsignal_t BasicNodeManager::addVehicleSignal = cComponent::registerSignal("traci.vehicle.add");
const simsignal_t BasicNodeManager::updateVehicleSignal = cComponent::registerSignal("traci.vehicle.update");
const simsignal_t BasicNodeManager::removeVehicleSignal = cComponent::registerSignal("traci.vehicle.remove");

void BasicNodeManager::initialize()
{
    Core* core = inet::getModuleFromPar<Core>(par("coreModule"), this);
    subscribeTraCI(core);
    m_api = &core->getLiteAPI();
    m_mapper = inet::getModuleFromPar<ModuleMapper>(par("mapperModule"), this);
    m_nodeIndex = 0;
    m_rsuIndex = 0;
    m_probeIndex = 0;
    m_vehicle_sink_module = par("vehicleSinkModule").stringValue();
    m_subscriptions = inet::getModuleFromPar<SubscriptionManager>(par("subscriptionsModule"), this);
    m_destroy_vehicles_on_crash = par("destroyVehiclesOnCrash");
    directional = par("directional");
}

void BasicNodeManager::finish()
{
    m_api = nullptr;
    unsubscribeTraCI();
    cSimpleModule::finish();
}

void BasicNodeManager::traciInit()
{
    m_boundary = Boundary { m_api->simulation().getNetBoundary() };
    m_subscriptions->subscribeSimulationVariables(sSimulationVariables);
    m_subscriptions->subscribeVehicleVariables(sVehicleVariables);

    // insert already running vehicles
    for (const std::string& id : m_api->vehicle().getIDList()) {
        addVehicle(id);
    }

    addRSUs();
}

void BasicNodeManager::traciStep()
{
    auto sim_cache = m_subscriptions->getSimulationCache();
    ASSERT(checkTimeSync(*sim_cache, omnetpp::simTime()));

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

    emit(updateNodeSignal, getNumberOfNodes());
}

void BasicNodeManager::traciClose()
{
    for (unsigned i = m_nodes.size(); i > 0; --i) {
        removeNodeModule(m_nodes.begin()->first);
    }
}

void BasicNodeManager::addRSUs()
{
    cXMLElement* config = par("RSUs").xmlValue();

    int index = 0;
    for (cXMLElement* rsu : config->getChildrenByTagName("rsu")) {

        TraCIPosition sumoPos;
        sumoPos.x = std::stod(rsu->getAttribute("positionX"));
        sumoPos.y = std::stod(rsu->getAttribute("positionY"));
        artery::Position omnetPos = position_cast(m_boundary, sumoPos);

        std::list<double> antennaDirections;

        for (cXMLElement* antenna : rsu->getChildrenByTagName("antenna")){
            double direction = std::stod(antenna->getAttribute("direction"));
            antennaDirections.push_back(direction);
        }

        RSU rsuStruct;
        rsuStruct.position = omnetPos;
        rsuStruct.antennaDirections = antennaDirections;

        rsuMap.insert(std::make_pair(std::to_string(index), rsuStruct));

        addRSU(std::to_string(index));
        index++;
    }
}

void BasicNodeManager::addRSU(const std::string& index)
{
    artery::Position omnetPos(rsuMap.at(index).position);
    std::list<double> antennaDirections = rsuMap.at(index).antennaDirections;


    std::string rsuID = "rsu" + index;
    cModuleType* type = cModuleType::get("artery.inet.RSU");
    cModule* rsuModule = createRSUModule(rsuID, type);

    rsuModule->par("numRadios") = (directional? antennaDirections.size() : 1);
    rsuModule->par("directionalAntennas") = (directional ? true : false);
    rsuModule->finalizeParameters();
    rsuModule->buildInside();
    m_nodes[rsuID] = rsuModule;
    rsuModule->scheduleStart(simTime());

    cModule* mobilityModule = rsuModule->getModuleByPath(m_vehicle_sink_module.c_str());
    auto* mobility = dynamic_cast<artery::RSUMobility*>(mobilityModule);
    mobility->setPosition(inet::Coord(omnetPos.x.value(), omnetPos.y.value(), 7.1));

    if(directional){
        int antennaMobilityIndex = 0;
        for (std::list<double>::iterator it = antennaDirections.begin(); it != antennaDirections.end(); it++){
            std::stringstream ss;
            ss << ".antennaMobility[" << antennaMobilityIndex << "]";
            std::string antennaMobilityPath = ss.str();
            cModule* antennaMobilityModule = rsuModule->getModuleByPath(antennaMobilityPath.c_str());
            auto* antennaMobility = dynamic_cast<artery::AntennaMobility*>(antennaMobilityModule);
            antennaMobility->setOffsetAlpha(*it);
            antennaMobilityIndex++;
        }
    }

    rsuModule->callInitialize();
    emit(addNodeSignal, rsuID.c_str(), rsuModule);
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

cModule* BasicNodeManager::createModule(const std::string&, cModuleType* type)
{
    cModule* module = type->create("node", getSystemModule(), m_nodeIndex, m_nodeIndex);
    ++m_nodeIndex;
    return module;
}

cModule* BasicNodeManager::createRSUModule(const std::string&, cModuleType* type)
{
    cModule* module = type->create("rsu", getSystemModule(), m_rsuIndex, m_rsuIndex);
    ++m_rsuIndex;
    return module;
}

cModule* BasicNodeManager::createProbeModule(const std::string&, cModuleType* type)
{
    cModule* module = type->create("probe", getSystemModule(), m_probeIndex, m_probeIndex);
    ++m_probeIndex;
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

} // namespace traci
