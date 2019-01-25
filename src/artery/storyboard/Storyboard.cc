#include "artery/storyboard/Storyboard.h"
#include "artery/storyboard/Story.h"
#include "artery/storyboard/Effect.h"
#include "artery/traci/VehicleController.h"
#include "inet/common/ModuleAccess.h"
#include "traci/Core.h"
#include "traci/Position.h"
#include <omnetpp/ccomponent.h>
#include <omnetpp/cexception.h>
#include <pybind11/embed.h>

using namespace omnetpp;
namespace py = pybind11;

namespace artery
{

Define_Module(Storyboard)

namespace
{

const auto traciAddNodeSignal = omnetpp::cComponent::registerSignal("traci.node.add");
const auto traciRemoveNodeSignal = omnetpp::cComponent::registerSignal("traci.node.remove");
const auto traciInitSignal = omnetpp::cComponent::registerSignal("traci.init");
const auto traciStepSignal = omnetpp::cComponent::registerSignal("traci.step");

class PythonContextImpl : public Storyboard::PythonContext
{
public:
    pybind11::module& module() override { return m_module; }

private:
    pybind11::scoped_interpreter m_interpreter;
    pybind11::module m_module;
};

} // namespace


Storyboard::Storyboard() :
    m_python(new PythonContextImpl())
{
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
        traci->subscribe(traciInitSignal, this);

        try {
            // Append directory containing omnetpp.ini to Python import path
            const auto& netConfigEntry = getEnvir()->getConfig()->getConfigEntry("network");
            const char* simBaseDir = netConfigEntry.getBaseDirectory();
            assert(simBaseDir);
            py::module::import("sys").attr("path").attr("append")(simBaseDir);
            EV_INFO << "Appended " << simBaseDir << " to Python system path" << endl;

            // Load module containing storyboard description
            m_python->module() = py::module::import(par("python").stringValue());
        } catch (const py::error_already_set&) {
            PyErr_Print();
            throw;
        }

        // Par visualisation flag from ned
        mDrawConditions = par("drawConditions");
    } else if(stage == 1) {
        std::string canvas = par("canvas");
        if(canvas == "storyboard") {
            mCanvas = getCanvas();
        } else {
            auto* module = getModuleByPath(canvas.c_str());
            if(!module) {
                throw cRuntimeError("No canvas found at %s. Check storyboard's canvas NED parameter!", canvas.c_str());
            } else {
                mCanvas = module->getCanvas();
            }
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
        artery::Middleware* appl = inet::findModuleFromPar<artery::Middleware>(par("middlewareModule"), nodeModule, false);
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

void Storyboard::receiveSignal(cComponent* source, simsignal_t signalId, const simtime_t&, cObject*)
{
    if (signalId == traciStepSignal) {
        updateStoryboard();
        if(mDrawConditions) {
            drawConditions();
        }
    }
    else if (signalId == traciInitSignal) {
        traci::Core* core = check_and_cast<traci::Core*>(source);
        const libsumo::TraCIPositionVector& boundary = core->getLiteAPI().simulation().getNetBoundary();
        mNetworkBoundary = traci::Boundary { boundary };

        try {
            py::object board = py::cast(this, py::return_value_policy::reference);
            m_python->module().attr("createStories")(board);
        } catch (const py::error_already_set&) {
            PyErr_Print();
            throw;
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

Position Storyboard::convertTraciPosition(double x, double y)
{
    libsumo::TraCIPosition traci;
    traci.x = x;
    traci.y = y;
    traci.z = 0.0;
    return position_cast(mNetworkBoundary, traci);
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

} // namespace artery

