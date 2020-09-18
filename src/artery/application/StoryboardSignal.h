#ifndef ARTERY_STORYBOARDSIGNAL_H_W6RAFKKY
#define ARTERY_STORYBOARDSIGNAL_H_W6RAFKKY

#include <omnetpp/cobject.h>
#include <set>
#include <string>

namespace artery
{

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

    omnetpp::cObject* dup() const override;

private:
    std::string mCause;
    Vehicle* mEgo;
    Story* mStory;
    std::set<const Vehicle*> mOthers;
};

} // namespace artery

#endif /* ARTERY_STORYBOARDSIGNAL_H_W6RAFKKY */

