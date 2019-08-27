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
    if (stage == InitStages::Prepare) {
        mDirectional = par("directional");
        mRsuPrefix = par("rsuPrefix").stdstringValue();
    } else if (stage == InitStages::Self) {
        loadRoadSideUnits();
    }
}

void StaticNodeManager::loadRoadSideUnits()
{
    cXMLElement* config = par("nodes").xmlValue();

    for (cXMLElement* rsu : config->getChildrenByTagName("rsu")) {
        const char* id = rsu->getAttribute("junctionID");
        if (!id) {
            EV_WARN << "rsu has no id specified, skip" << endl;
        } else if (mRsuMap.find(id) != mRsuMap.end()) {
            EV_WARN << "rsu with id " << id << " already exists, skip" << endl;
        }

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

        mRsuMap.insert(std::make_pair(id, rsuStruct));
        addRoadSideUnit(id);
    }
}

void StaticNodeManager::addRoadSideUnit(const std::string& id)
{
    artery::Position& omnetPos = mRsuMap.at(id).position;
    std::list<double>& antennaDirections = mRsuMap.at(id).antennaDirections;

    cModule* rsuModule = createRoadSideUnitModule(id);
    rsuModule->par("numRadios") = (mDirectional? antennaDirections.size() : 1);
    rsuModule->par("directionalAntennas") = (mDirectional ? true : false);
    rsuModule->finalizeParameters();
    rsuModule->buildInside();

    cModule* mobilityModule = rsuModule->getSubmodule("mobility");
    if (mobilityModule) {
        const int idxParInitFromDisplayString = mobilityModule->findPar("initFromDisplayString");
        if (idxParInitFromDisplayString >= 0) {
            mobilityModule->par(idxParInitFromDisplayString) = false;
        }

        const int idxParX = mobilityModule->findPar("initialX");
        if (idxParX >= 0) {
            mobilityModule->par(idxParX) = omnetPos.x.value();
        } else {
            error("RSU module type has no initialX parameter");
        }

        const int idxParY = mobilityModule->findPar("initialY");
        if (idxParY >= 0) {
            mobilityModule->par(idxParY) = omnetPos.y.value();
        } else {
            error("RSU module type has no initialY parameter");
        }

        const int idxParZ = mobilityModule->findPar("initialZ");
        if (idxParZ >= 0) {
            mobilityModule->par(idxParZ) = par("rsuHeight");
        } else {
            error("RSU module type has no initialZ parameter");
        }
    } else {
        error("no mobility submodule found in RSU node");
    }

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

    rsuModule->scheduleStart(simTime());
    for (int i = 0; i < InitStages::Self; ++i) {
        if (!rsuModule->callInitialize(i)) break;
    }
    emit(addRoadSideUnitSignal, id.c_str(), rsuModule);
}

cModule* StaticNodeManager::createRoadSideUnitModule(const std::string& id)
{
    cModuleType* type = cModuleType::get(par("rsuType"));
    std::string name = mRsuPrefix + id;
    cModule* module = type->create(name.c_str(), getSystemModule());
    return module;
}

} // namespace artery
