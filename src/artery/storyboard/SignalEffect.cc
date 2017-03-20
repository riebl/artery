#include "artery/storyboard/SignalEffect.h"
#include "artery/storyboard/StoryboardSignal.h"

using namespace omnetpp;

namespace {

class Visitor : public boost::static_visitor<const std::set<const Vehicle*>&>
{
public:
    const std::set<const Vehicle*>& operator()(bool) const
    {
        static const std::set<const Vehicle*> empty;
        return empty;
    }

    const std::set<const Vehicle*>& operator()(std::set<const Vehicle*>& s) const
    {
        return s;
    }
};

} // namespace

void SignalEffect::applyEffect()
{
    StoryboardSignal obj;
    obj.setStory(&getStory());
    obj.setCause(mTriggerCause);
    obj.setEgoVehicle(&getCar());
    obj.setOtherVehicles(boost::apply_visitor(Visitor(), mResult));

    getCar().emit(obj);
}
