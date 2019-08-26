#include "artery/StaticNodeManager.h"
#include "artery/inet/AntennaMobility.h"
#include "artery/utility/InitStages.h"

namespace artery
{

Define_Module(StaticNodeManager)

const simsignal_t StaticNodeManager::addRoadSideUnitSignal = cComponent::registerSignal("addRoadSideUnit");


int StaticNodeManager::numInitStages() const
{
    return InitStages::Total;
}

void StaticNodeManager::initialize(int stage)
{
    if (stage == InitStages::Self) {
        mRsuIndex = 0;
        mProbeIndex = 0;
        mDirectional = par("directional");
    } else if (stage == InitStages::Prepare) {
        loadRoadSideUnits();
    }
}

void StaticNodeManager::loadRoadSideUnits()
{
    cXMLElement* config = par("nodes").xmlValue();

    int index = 0;
    for (cXMLElement* rsu : config->getChildrenByTagName("rsu")) {
        Position pos;
        pos.x = std::stod(rsu->getAttribute("positionX")) * boost::units::si::meter;
        pos.y = std::stod(rsu->getAttribute("positionY")) * boost::units::si::meter;

        std::list<double> antennaDirections;

        for (cXMLElement* antenna : rsu->getChildrenByTagName("antenna")){
            double direction = std::stod(antenna->getAttribute("direction"));
            antennaDirections.push_back(direction);
        }

        RSU rsuStruct;
        rsuStruct.position = pos;
        rsuStruct.antennaDirections = antennaDirections;

        mRsuMap.insert(std::make_pair(std::to_string(index), rsuStruct));

        addRoadSideUnit(std::to_string(index));
        index++;
    }
}

void StaticNodeManager::addRoadSideUnit(const std::string& index)
{
    artery::Position omnetPos(mRsuMap.at(index).position);
    std::list<double> antennaDirections = mRsuMap.at(index).antennaDirections;

    std::string rsuID = "rsu" + index;
    cModuleType* type = cModuleType::get(par("rsuType"));
    cModule* rsuModule = createRSUModule(rsuID, type);

    rsuModule->par("numRadios") = (mDirectional? antennaDirections.size() : 1);
    rsuModule->par("directionalAntennas") = (mDirectional ? true : false);

    const int idxParX = rsuModule->findPar("initialX");
    if (idxParX >= 0) {
        rsuModule->par(idxParX) = omnetPos.x.value();
    } else {
        error("RSU module type has no initialX parameter");
    }

    const int idxParY = rsuModule->findPar("initialY");
    if (idxParY >= 0) {
        rsuModule->par(idxParY) = omnetPos.y.value();
    } else {
        error("RSU module type has no initialY parameter");
    }

    const int idxParZ = rsuModule->findPar("initialZ");
    if (idxParZ >= 0) {
        rsuModule->par(idxParZ) = par("rsuHeight");
    } else {
        error("RSU module type has no initialZ parameter");
    }

    rsuModule->finalizeParameters();
    rsuModule->buildInside();
    rsuModule->scheduleStart(simTime());

    if (mDirectional) {
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
    emit(addRoadSideUnitSignal, rsuID.c_str(), rsuModule);
}

cModule* StaticNodeManager::createRSUModule(const std::string&, cModuleType* type)
{
    cModule* module = type->create("rsu", getSystemModule(), mRsuIndex, mRsuIndex);
    ++mRsuIndex;
    return module;
}

cModule* StaticNodeManager::createProbeModule(const std::string&, cModuleType* type)
{
    cModule* module = type->create("probe", getSystemModule(), mProbeIndex, mProbeIndex);
    ++mProbeIndex;
    return module;
}

} // namespace artery
