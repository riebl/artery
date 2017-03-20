#ifndef STORYBOARDSIGNAL_H_W6RAFKKY
#define STORYBOARDSIGNAL_H_W6RAFKKY

#include "artery/storyboard/Condition.h"
#include <omnetpp/cobject.h>
#include <set>
#include <string>

class Story;
class Vehicle;


class StoryboardSignal : public omnetpp::cObject
{
public:
    StoryboardSignal();

    void setCause(const std::string&);
    const std::string& getCause() const;

    void setEgoVehicle(Vehicle*);
    const Vehicle* getEgoVehicle() const;

    void setStory(Story*);
    const Story* getStory() const;

    void setOtherVehicles(const std::set<const Vehicle*>&);
    const std::set<const Vehicle*> getOtherVehicles() const;

private:
    std::string mCause;
    Vehicle* mEgo;
    Story* mStory;
    std::set<const Vehicle*> mOthers;
};

#endif /* STORYBOARDSIGNAL_H_W6RAFKKY */

