#include "artery/application/LocalDynamicMap.h"
#include "artery/application/Timer.h"
#include <omnetpp/csimulation.h>
#include <algorithm>

namespace artery
{

LocalDynamicMap::LocalDynamicMap(const Timer& timer) :
    mTimer(timer)
{
}

void LocalDynamicMap::updateAwareness(const Cam& msg)
{
    auto tai = mTimer.reconstructMilliseconds(msg->cam.generationDeltaTime);
    auto sim = mTimer.getTimeFor(tai);
    AwarenessEntry entry;
    entry.last_cam = msg;
    entry.expiry = sim + omnetpp::SimTime { 1100, omnetpp::SIMTIME_MS };
    assert(entry.expiry > simTime() && entry.expiry < simTime() + 2.0);
    mCaMessages[msg->header.stationID] = std::move(entry);
}

void LocalDynamicMap::dropExpired()
{
    auto now = mTimer.getCurrentTime();

    for (auto it = mCaMessages.begin(); it != mCaMessages.end();) {
        if (it->second.expiry < omnetpp::simTime()) {
            it = mCaMessages.erase(it);
        } else {
            ++it;
        }
    }
}

unsigned LocalDynamicMap::count(const CamPredicate& cp) const
{
    return std::count_if(mCaMessages.begin(), mCaMessages.end(),
            [&cp](const std::pair<StationID, AwarenessEntry>& map_entry) {
                return cp(map_entry.second.last_cam);
            });
}

} // namespace artery
