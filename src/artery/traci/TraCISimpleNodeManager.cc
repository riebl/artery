#include "artery/traci/TraCISimpleNodeManager.h"
#include "inet/common/ModuleAccess.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"
#include "veins/modules/mobility/traci/TraCIModuleMapper.h"

TraCISimpleNodeManager::TraCISimpleNodeManager(const Veins::TraCIModuleMapper& mapper, cModule* parent) :
    m_moduleMapper(mapper), m_moduleParent(parent), m_moduleIndex(0)
{
}

void TraCISimpleNodeManager::setMobilityModule(cPar& par)
{
	m_moduleMobility = par;
}

void TraCISimpleNodeManager::add(const std::string& nodeId, const NodeData& data, const std::string& vehicleType, simtime_t start)
{
	const auto moduleConfig = m_moduleMapper.getModuleConfig(vehicleType);
	if (moduleConfig.type == "") return;

	cModuleType* moduleType = cModuleType::get(moduleConfig.type.c_str());
	if (!moduleType)
		throw cRuntimeError("module type \"%\" not found", moduleConfig.type.c_str());

	cModule* node = moduleType->create(moduleConfig.name.c_str(), m_moduleParent, m_moduleIndex, m_moduleIndex);
	++m_moduleIndex;
	node->finalizeParameters();
	node->setDisplayString(moduleConfig.displayString.c_str());
	node->buildInside();
	node->scheduleStart(start);

	preInitialize(node, nodeId, data);
	node->callInitialize();
	postInitialize(node, nodeId, data);
	m_nodes[nodeId] = node;
}

void TraCISimpleNodeManager::remove(const std::string& nodeId)
{
	std::map<std::string, cModule*>::iterator found = m_nodes.find(nodeId);
	if (found != m_nodes.end()) {
		cModule* node = found->second;
		m_nodes.erase(found);
		node->callFinish();
		node->deleteModule();
	} else {
		throw cRuntimeError("no vehicle with id \"%s\" found", nodeId.c_str());
	}
}

void TraCISimpleNodeManager::update(const std::string& nodeId, const NodeData& data)
{
	cModule* mod = m_nodes.at(nodeId);
	auto mob = getMobility(mod);
	if (mob) {
		EV << "module " << nodeId << " moving to " << data.position.x << "," << data.position.y << endl;
		mob->nextPosition(data.position, data.road_id, data.speed, data.angle, data.signals);
	}
}

cModule* TraCISimpleNodeManager::get(const std::string& nodeId)
{
	cModule* node = nullptr;
	auto found = m_nodes.find(nodeId);
	if (found != m_nodes.end()) {
		node = found->second;
	}
	return node;
}

void TraCISimpleNodeManager::finish()
{
	while (m_nodes.begin() != m_nodes.end()) {
		remove(m_nodes.begin()->first);
	}
}

size_t TraCISimpleNodeManager::size()
{
	return m_nodes.size();
}

Veins::TraCIMobility* TraCISimpleNodeManager::getMobility(cModule* node)
{
	if (!m_moduleMobility) {
		throw cRuntimeError("missing setMobilityModule call");
	}
	return inet::getModuleFromPar<Veins::TraCIMobility>(*m_moduleMobility, node);
}

void TraCISimpleNodeManager::preInitialize(cModule* node, const std::string& nodeId, const NodeData& data)
{
	auto mob = getMobility(node);
	mob->preInitialize(nodeId, data.position, data.road_id, data.speed, data.angle);
}

void TraCISimpleNodeManager::postInitialize(cModule* node, const std::string& nodeId, const NodeData& data)
{
	auto mob = getMobility(node);
	mob->changePosition();
}
