#include "artery/application/LocalDynamicMap.h"
#include "artery/application/Timer.h"
#include <omnetpp/csimulation.h>
#include <cassert>
#include <algorithm>

namespace artery
{

LocalDynamicMap::LocalDynamicMap(const Timer& timer) :
    mTimer(timer)
{
}

void LocalDynamicMap::updateAwareness(const CaObject& obj)
{
    const vanetza::asn1::Cam& msg = obj.asn1();

    static const omnetpp::SimTime lifetime { 1100, omnetpp::SIMTIME_MS };
    auto tai = mTimer.reconstructMilliseconds(msg->cam.generationDeltaTime);
    const omnetpp::SimTime expiry = mTimer.getTimeFor(tai) + lifetime;

    const auto now = omnetpp::simTime();
    if (expiry < now || expiry > now + 2 * lifetime) {
        EV_STATICCONTEXT
        EV_WARN << "Expiry of received CAM is out of bounds";
        return;
    }

    AwarenessEntry entry(obj, expiry);
    auto found = mCaMessages.find(msg->header.stationID);
    if (found != mCaMessages.end()) {
        found->second = std::move(entry);
    } else {
        mCaMessages.emplace(msg->header.stationID, std::move(entry));
    }
}

void LocalDynamicMap::dropExpired()
{
    const auto now = omnetpp::simTime();
    for (auto it = mCaMessages.begin(); it != mCaMessages.end();) {
        if (it->second.expiry < now) {
            it = mCaMessages.erase(it);
        } else {
            ++it;
        }
    }
}

unsigned LocalDynamicMap::count(const CamPredicate& predicate) const
{
    return std::count_if(mCaMessages.begin(), mCaMessages.end(),
            [&predicate](const std::pair<const StationID, AwarenessEntry>& map_entry) {
                const Cam& cam = map_entry.second.object.asn1();
                return predicate(cam);
            });
}

LocalDynamicMap::AwarenessEntry::AwarenessEntry(const CaObject& obj, omnetpp::SimTime t) :
    expiry(t), object(obj)
{
}

} // namespace artery
