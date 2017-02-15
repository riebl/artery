#ifndef STORYBOARD_H_
#define STORYBOARD_H_

#include <boost/python.hpp>
#include <map>
#include <omnetpp.h>
#include "artery/application/ItsG5Middleware.h"
#include "artery/storyboard/EffectStack.h"
#include "artery/storyboard/Vehicle.h"

class Effect;
class Story;

namespace traci { class VehicleController; }

class Storyboard : public cSimpleModule, public cListener
{
private:

    /**
     * Initializes the Storyboard
     * Handled by Omnet++
     */
    virtual void initialize(int) override;

    /**
     * Recieves messages, sent to the Storyboard
     * Handled by Omnet++
     */
    virtual void handleMessage(cMessage * msg);

    void receiveSignal(cComponent* source, simsignal_t, const char*, cObject*) override;
    void receiveSignal(cComponent* source, simsignal_t, const simtime_t&, cObject*) override;

    boost::python::object module;
    std::vector<std::shared_ptr<Story>> m_stories;
    std::map<traci::VehicleController*, EffectStack> m_affectedCars;
    std::map<std::string, Vehicle> m_vehicles;

public:
    /**
     * Updates the storyboard by checking all stories
     * Is called each time TraCIScenarioManager processes one omnet step
     */
    void updateStoryboard();

    /**
     * Registers a story created in the python script
     * \param shared_ptr to a story, which should be executed from the storyboard
     */
    void registerStory(std::shared_ptr<Story>);

    /**
     * Adds all effects generated from a story
     * \param list all effects to add, all effects needs to be from the same story and the same car
     */
    void addEffect(const std::vector<std::shared_ptr<Effect>>&);

    /**
     * Removes all Effects from one car related to one Story
     * \param Vehicle from which the Effects should be removed
     * \param Story to remove
     */
    void removeStory(traci::VehicleController*, const Story*);

    /**
     * Checks if a specific Story is already applied on a TraCIMobility
     * \param Vehicle which should be tested
     * \param Story that should be tested
     */
    bool storyApplied(traci::VehicleController*, const Story*);

    /**
     * Checks if the story has to be applied or removed
     * Is called from Storyboard::update()
     * param: Vehicle which should be tested if the story is already applied
     * param: bool result of condition test
     * param: Story which was tested in the update function
     */
    void checkCar(traci::VehicleController&, bool, Story*);
};

#endif /* STORYBOARD_H_ */
