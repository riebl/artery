#include "artery/storyboard/Storyboard.h"
#include "artery/storyboard/PythonModule.h"
#include "artery/storyboard/Story.h"
#include "artery/storyboard/Effect.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"

Define_Module(Storyboard);

void Storyboard::initialize(int stage)
{
    // Get TraCiScenarioManager
    manager = static_cast<Veins::TraCIScenarioManager*>(this->getModuleByPath("^.manager"));
    assert(manager);

    // Import staticly linked modules
    PyImport_AppendInittab("storyboard", &initstoryboard);
    PyImport_AppendInittab("timeline", &inittimeline);

    // Initialize python
    setenv("PYTHONPATH", ".", 1);
    Py_Initialize();

    try {
        // Load storyboard.py
        module = python::import("demo");
        module.attr("board") = boost::cref(this);
        module.attr("createStories") ();

    } catch (const python::error_already_set&) {
        PyErr_Print();
    }
}

void Storyboard::handleMessage(cMessage * msg)
{
    // Storyboard does not expect any messages at the moment
    EV << "Message arrived \n";
}

cModule* Storyboard::getParentModule(cModule* mod)
{
    return mod->getParentModule();
}

void Storyboard::updateStoryboard()
{
    // iterate through all cars
    for (auto& car : getCars()) {
        // test all story conditions for each story
        for (auto& story : m_stories) {
            bool conditionTest = true;
            if(!story->testCondition(car)) {
                conditionTest = false;
            }
            // check if the story has to be applied or removed
            checkCar(car.mobility, conditionTest, story.get());
        }
    }
}

void Storyboard::checkCar(Veins::TraCIMobility& car, bool conditionsPassed, Story* story)
{
    // If the Story is already applied on this car and the condition test is not passed
    // remove Story from EffectStack, because the car left the affected area
    if (storyApplied(&car, story)) {
        if (!conditionsPassed) {
            removeStory(&car, story);
        }
    }
    // Story was not applied on this car and condition test is passed
    // results in adding all effects from the Story to the car
    else {
        if (conditionsPassed) {
            std::vector<std::shared_ptr<Effect>> effects;
            for (auto factory : story->getEffectFactories()) {
                effects.push_back(std::move(factory->create(car, story)));
            }
            addEffect(effects);
        }
    }
}

std::vector<Vehicle> Storyboard::getCars() {
    std::vector<Vehicle> vec;
    for (auto host : manager->getManagedHosts()) {
        ItsG5Middleware* appl = dynamic_cast<ItsG5Middleware*>(host.second->getSubmodule("appl"));
        if (appl == nullptr) {
            opp_error("Could not find ItsG5Middleware submodule");
        }
        Facilities* f = appl->getFacilities();
        Vehicle v(f->getMobility(), f->getVehicleDataProvider());
        vec.push_back(v);
    }
    return vec;
}

void Storyboard::registerStory(std::shared_ptr<Story> story)
{
    m_stories.push_back(story);
}

bool Storyboard::storyApplied(Veins::TraCIMobility* car, const Story* story)
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
    Veins::TraCIMobility* car = effects.begin()->get()->getCar();
    // No EffectStack found for this car or the story is not applied yet
    // apply effect
    if (m_affectedCars.count(car) == 0 || !m_affectedCars[car].isStoryOnStack(effects.begin()->get()->getStory()) ) {
        for(auto effect : effects) {
            m_affectedCars[car].addEffect(std::move(effect));
            EV << "Effect added for: " << car->getExternalId() << "\n";
        }
    }
    // Effect is already on Stack -> should never happen
    else {
        opp_error("It's not allowed to add effects from a Story twice");
    }
}

void Storyboard::removeStory(Veins::TraCIMobility* car, const Story* story)
{
    m_affectedCars[car].removeEffectsByStory(story);
}
