#include "artery/storyboard/Storyboard.h"
#include "artery/storyboard/PythonModule.h"
#include "artery/storyboard/Story.h"
#include "artery/storyboard/Effect.h"
#include "artery/traci/VehicleController.h"
#include "inet/common/ModuleAccess.h"
#include <omnetpp/ccomponent.h>
#include <omnetpp/cexception.h>

using namespace omnetpp;

Define_Module(Storyboard)

namespace
{

const auto traciAddNodeSignal = omnetpp::cComponent::registerSignal("traci.node.add");
const auto traciRemoveNodeSignal = omnetpp::cComponent::registerSignal("traci.node.remove");
const auto traciStepSignal = omnetpp::cComponent::registerSignal("traci.step");

}


void Storyboard::initialize(int stage)
{
    if(stage == 0) {
        // Get TraCiScenarioManager
        cModule* traci = getModuleByPath(par("traciModule").stringValue());
        if (!traci) {
            throw cRuntimeError("No TraCI module found for signal subscription at %s", par("traciModule").stringValue());
        }

        traci->subscribe(traciAddNodeSignal, this);
        traci->subscribe(traciRemoveNodeSignal, this);
        traci->subscribe(traciStepSignal, this);

        // Import staticly linked modules
#       if PY_VERSION_HEX >= 0x03000000
        PyImport_AppendInittab("storyboard", &PyInit_storyboard);
        PyImport_AppendInittab("timeline", &PyInit_timeline);
#       else
        PyImport_AppendInittab("storyboard", &initstoryboard);
        PyImport_AppendInittab("timeline", &inittimeline);
#       endif

        // Initialize python
        Py_Initialize();

        try {
            // Append directory containing omnetpp.ini to Python import path
            const auto& netConfigEntry = getEnvir()->getConfig()->getConfigEntry("network");
            const char* simBaseDir = netConfigEntry.getBaseDirectory();
            assert(simBaseDir);
            python::import("sys").attr("path").attr("append")(simBaseDir);
            EV_INFO << "Appended " << simBaseDir << " to Python system path" << endl;

            // Load module containing storyboard description
            module = python::import(par("python").stringValue());
            module.attr("board") = boost::cref(*this);
            module.attr("createStories") ();

        } catch (const python::error_already_set&) {
            PyErr_Print();
            throw;
        }

        // Par visualisation flag from ned
        mDrawConditions = par("drawConditions");
    } else if(stage == 1){
        std::string canvas = par("canvas");
        if(canvas == "storyboard") {
            mCanvas = getCanvas();
        } else {
            mCanvas = getModuleByPath(canvas.c_str())->getCanvas();
        }
    }
}

void Storyboard::handleMessage(cMessage * msg)
{
    // Storyboard does not expect any messages at the moment
}

void Storyboard::receiveSignal(cComponent* source, simsignal_t signalId, const char* nodeId, cObject* node)
{
    if (signalId == traciAddNodeSignal) {
        cModule* nodeModule = dynamic_cast<cModule*>(node);
        ItsG5Middleware* appl = inet::findModuleFromPar<ItsG5Middleware>(par("middlewareModule"), nodeModule, false);
        if (appl) {
            m_vehicles.emplace(nodeId, Vehicle { *appl, m_vehicles });
        } else {
            EV_DEBUG << "Node " << nodeId << " is not equipped with middleware module, skipped by Storyboard" << endl;
        }
    }
    else if (signalId == traciRemoveNodeSignal) {
        m_vehicles.erase(nodeId);
    }
}

void Storyboard::receiveSignal(cComponent*, simsignal_t signalId, const simtime_t&, cObject*)
{
    if (signalId == traciStepSignal) {
        updateStoryboard();
        if(mDrawConditions) {
            drawConditions();
        }
    }
}

void Storyboard::updateStoryboard()
{
    // iterate through all cars
    for (auto& car : m_vehicles) {
        // test all story conditions for each story
        for (auto& story : m_stories) {
            ConditionResult conditionTest = story->testCondition(car.second);
            // check if the story has to be applied or removed
            checkCar(car.second, conditionTest, story.get());
        }
    }
}

void Storyboard::drawConditions()
{
    if (mCanvas != nullptr) {
        for (auto& story : m_stories) {
            story->getCondition()->drawCondition(mCanvas);
        }
    }
}

void Storyboard::checkCar(Vehicle& car, ConditionResult& conditionResult, Story* story)
{
    // If the Story is already applied on this car and the condition test is not passed
    // remove Story from EffectStack, because the car left the affected area
    if (storyApplied(&car, story)) {
        if (!is_true(conditionResult)) {
            removeStory(&car, story);
        }
    }
    // Story was not applied on this car and condition test is passed
    // results in adding all effects from the Story to the car
    else if (is_true(conditionResult)) {
        std::vector<std::shared_ptr<Effect>> effects;
        for (auto factory : story->getEffectFactories()) {
            effects.push_back(factory->create(car, *story, conditionResult));
        }
        addEffect(effects);
    }
}

void Storyboard::registerStory(std::shared_ptr<Story> story)
{
    m_stories.push_back(story);
}

bool Storyboard::storyApplied(Vehicle* car, const Story* story)
{
    if (m_affectedCars.count(car) == 0) {
        return false;
    }
    else {
        return m_affectedCars[car].isStoryOnStack(story);
    }
}

void Storyboard::addEffect(const std::vector<std::shared_ptr<Effect>>& effects)
{
    Vehicle& car = effects.front()->getCar();
    Story& story = effects.front()->getStory();
    // No EffectStack found for this car or the story is not applied yet
    // apply effect
    if (m_affectedCars.count(&car) == 0 || !m_affectedCars[&car].isStoryOnStack(&story) ) {
        for(auto effect : effects) {
            m_affectedCars[&car].addEffect(std::move(effect));
            EV_DEBUG << "Effect added for: " << car.getId() << endl;
        }
    }
    // Effect is already on Stack -> should never happen
    else {
        throw cRuntimeError("It's not allowed to add effects from a Story twice");
    }
}

void Storyboard::removeStory(Vehicle* car, const Story* story)
{
    m_affectedCars[car].removeEffectsByStory(story);
}

int Storyboard::numInitStages() const
{
    return 2;
}
