#ifndef ARTERY_STORYBOARD_H_6AOIDBWG
#define ARTERY_STORYBOARD_H_6AOIDBWG

#include <map>
#include <memory>
#include <omnetpp/ccanvas.h>
#include <omnetpp/clistener.h>
#include <omnetpp/csimplemodule.h>
#include "artery/storyboard/Condition.h"
#include "artery/storyboard/EffectStack.h"
#include "artery/storyboard/Macros.h"
#include "artery/storyboard/Vehicle.h"
#include "artery/utility/Geometry.h"
#include "traci/Boundary.h"

namespace pybind11 { class module_; }

namespace artery
{

class Effect;
class Story;
class Vehicle;

class STORYBOARD_API Storyboard : public omnetpp::cSimpleModule, public omnetpp::cListener
{
public:
    Storyboard();

    // omnetpp::cSimpleModule
    void initialize(int) override;
    int numInitStages() const override;
    void handleMessage(omnetpp::cMessage * msg) override;
    // omnetpp::cListener
    void receiveSignal(omnetpp::cComponent* source, omnetpp::simsignal_t, const char*, omnetpp::cObject*) override;
    void receiveSignal(omnetpp::cComponent* source, omnetpp::simsignal_t, const omnetpp::SimTime&, omnetpp::cObject*) override;

    /**
     * Registers a story created in the python script
     * \param shared_ptr to a story, which should be executed from the storyboard
     */
    void registerStory(std::shared_ptr<Story>);

    /**
     * Convert a TraCI position to an OMNeT++ position
     */
    Position convertTraciPosition(double x, double y);

    class PythonContext
    {
    public:
        virtual pybind11::module_& module() = 0;
        virtual ~PythonContext() = default;
    };

private:
    /**
     * Updates the storyboard by checking all stories
     * Is called each time TraCIScenarioManager processes one omnet step
     */
    void STORYBOARD_LOCAL updateStoryboard();

    /**
     * Adds all effects generated from a story
     * \param list all effects to add, all effects needs to be from the same story and the same car
     */
    void STORYBOARD_LOCAL addEffect(const std::vector<std::shared_ptr<Effect>>&);

    /**
     * Removes all Effects from one car related to one Story
     * \param Vehicle from which the Effects should be removed
     * \param Story to remove
     */
    void STORYBOARD_LOCAL removeStory(Vehicle*, const Story*);

    /**
     * Checks if a specific Story is already applied on a TraCIMobility
     * \param Vehicle which should be tested
     * \param Story that should be tested
     */
    bool STORYBOARD_LOCAL storyApplied(Vehicle*, const Story*);

    /**
     * Checks if the story has to be applied or removed
     * Is called from Storyboard::update()
     * param: Vehicle which should be tested if the story is already applied
     * param: bool result of condition test
     * param: Story which was tested in the update function
     */
    void STORYBOARD_LOCAL checkCar(Vehicle&, ConditionResult&, Story*);

    /**
     * Iterate over all conditions associated with registered stories and draw them on canvas
     */
    void STORYBOARD_LOCAL drawConditions();

    std::unique_ptr<PythonContext> m_python;
    std::vector<std::shared_ptr<Story>> m_stories;
    std::map<Vehicle*, EffectStack> m_affectedCars;
    std::map<std::string, Vehicle> m_vehicles;
    bool mDrawConditions;
    omnetpp::cCanvas* mCanvas = nullptr;
    traci::Boundary mNetworkBoundary;
};

} // namespace artery

#endif /* ARTERY_STORYBOARD_H_6AOIDBWG */

