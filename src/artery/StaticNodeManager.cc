/*
* Artery V2X Simulation Framework
* Copyright 2019 Caglar Kaya, Alexander Willecke, Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/StaticNodeManager.h"
#include "artery/inet/AntennaMobility.h"
#include "artery/utility/InitStages.h"

namespace artery
{

Define_Module(StaticNodeManager)

// emitted signals
const simsignal_t StaticNodeManager::addRoadSideUnitSignal = cComponent::registerSignal("addRoadSideUnit");

namespace {
    // subscribed signals
    const simsignal_t initSignal = cComponent::registerSignal("traci.init");
} // namespace


StaticNodeManager::StaticNodeManager() :
    mInsertionEvent(new omnetpp::cMessage("insert static node"))
{
}

StaticNodeManager::~StaticNodeManager()
{
    cancelAndDelete(mInsertionEvent);
}

int StaticNodeManager::numInitStages() const
{
    return InitStages::Total;
}

void StaticNodeManager::initialize(int stage)
{
    if (stage == InitStages::Prepare) {
        mDirectionalAntennas = par("directionalAntennas");
        mRsuPrefix = par("rsuPrefix").stdstringValue();
    } else if (stage == InitStages::Self) {
        if (par("waitForTraCI")) {
            getSystemModule()->subscribe(initSignal, this);
        } else {
            loadRoadSideUnits();
        }
    }
}

void StaticNodeManager::handleMessage(omnetpp::cMessage* msg)
{
    if (msg == mInsertionEvent) {
        for (auto it = mInsertionQueue.begin(); it != mInsertionQueue.end();) {
            if (it->first <= simTime()) {
                addRoadSideUnit(it->second);
                it = mInsertionQueue.erase(it);
            } else {
                break;
            }
        }

        scheduleInsertionEvent();
    }
}

void StaticNodeManager::receiveSignal(omnetpp::cComponent* src, omnetpp::simsignal_t signal, const omnetpp::SimTime&, omnetpp::cObject*)
{
    if (signal == initSignal) {
        mInitSource = src;
        loadRoadSideUnits();
    }
}

void StaticNodeManager::scheduleInsertionEvent()
{
    cancelEvent(mInsertionEvent);
    if (!mInsertionQueue.empty()) {
        scheduleAt(mInsertionQueue.begin()->first, mInsertionEvent);
    }
}

void StaticNodeManager::loadRoadSideUnits()
{
    cXMLElement* config = par("nodes").xmlValue();

    for (cXMLElement* rsu : config->getChildrenByTagName("rsu")) {
        const char* id = rsu->getAttribute("id");
        if (!id) {
            EV_WARN << "rsu has no id specified, skip" << endl;
        } else if (mRsuMap.find(id) != mRsuMap.end()) {
            EV_WARN << "rsu with id " << id << " already exists, skip" << endl;
        }

        RSU rsuStruct;
        rsuStruct.position.x = std::stod(rsu->getAttribute("positionX")) * boost::units::si::meter;
        rsuStruct.position.y = std::stod(rsu->getAttribute("positionY")) * boost::units::si::meter;
        for (cXMLElement* antenna : rsu->getChildrenByTagName("antenna")) {
            double direction = std::stod(antenna->getAttribute("direction"));
            rsuStruct.antennaDirections.push_back(direction);
        }

        mRsuMap.emplace(id, std::move(rsuStruct));
        mInsertionQueue.emplace(simTime() + par("insertionDelay"), id);
    }

    scheduleInsertionEvent();
}

void StaticNodeManager::addRoadSideUnit(const std::string& id)
{
    const artery::Position& pos = mRsuMap.at(id).position;
    const std::vector<double>& antennaDirections = mRsuMap.at(id).antennaDirections;

    cModule* rsuModule = createRoadSideUnitModule(id);
    rsuModule->par("numRadios") = mDirectionalAntennas ? std::max(1ul, antennaDirections.size()) : 1;
    rsuModule->par("withAntennaMobility") = mDirectionalAntennas;
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
            mobilityModule->par(idxParX) = pos.x.value();
        } else {
            error("RSU module type has no initialX parameter");
        }

        const int idxParY = mobilityModule->findPar("initialY");
        if (idxParY >= 0) {
            mobilityModule->par(idxParY) = pos.y.value();
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

    if (mDirectionalAntennas) {
        for (std::size_t i = 0; i < antennaDirections.size(); ++i) {
            auto* antennaMobilityModule = rsuModule->getSubmodule("antennaMobility", i);
            if (!antennaMobilityModule) {
                error("missing antenna mobility submodule in RSU node at index %i", i);
            }

            const int idxParOffsetAlpha = antennaMobilityModule->findPar("offsetAlpha");
            if (idxParOffsetAlpha) {
                antennaMobilityModule->par(idxParOffsetAlpha) = antennaDirections[i];
            } else {
                error("%s has no offsetAlpha parameter", antennaMobilityModule->getFullPath().c_str());
            }
        }
    }

    rsuModule->scheduleStart(simTime());
    rsuModule->callInitialize();
    emit(addRoadSideUnitSignal, id.c_str(), rsuModule);

    // fake traci.init signal for added RSU module if it has matching listeners
    if (mInitSource) {
        auto listeners = getSystemModule()->getLocalSignalListeners(initSignal);
        for (omnetpp::cIListener* listener : listeners) {
            auto mod = dynamic_cast<omnetpp::cModule*>(listener);
            if (mod && rsuModule->containsModule(mod)) {
                listener->receiveSignal(mInitSource, initSignal, simTime(), nullptr);
            }
        }
    }
}

cModule* StaticNodeManager::createRoadSideUnitModule(const std::string& id)
{
    cModuleType* type = cModuleType::get(par("rsuType"));
    std::string name = mRsuPrefix + id;
    cModule* module = type->create(name.c_str(), getSystemModule());
    return module;
}

} // namespace artery
