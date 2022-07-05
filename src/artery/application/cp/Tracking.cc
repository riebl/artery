/*
 * Artery V2X Simulation Framework
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/cp/Tracking.h"

#include "artery/envmod/sensor/Sensor.h"

#include <functional>


namespace opp = omnetpp;


namespace artery
{
namespace cp
{

RemoteDynamics::RemoteDynamics() : mCam(nullptr), mHistoryCpmDynamics(nullptr)
{
}

RemoteDynamics::RemoteDynamics(const vanetza::asn1::Cam* cam) : mCam(cam), mHistoryCpmDynamics(nullptr)
{
}


std::pair<CpmDynamics, RemoteDynamics::CpmResult> RemoteDynamics::addCpmDynamics(StationID_t stationId, const CpmDynamics& cpmDynamics, CpmOption option)
{
    // This assumes the new element is newer than all present elements
    auto entry = getHistoryCpmDynamicsEntry(stationId, cpmDynamics.received, option);
    if (entry.second != CpmResult::DuplicateDrop) {
        entry.first.push_back(HistoryCpmDynamics{cpmDynamics.received, cpmDynamics.senderPosition});
        if (!mHistoryCpmDynamics || mHistoryCpmDynamics->received < entry.first.back().received) {
            mHistoryCpmDynamics = &entry.first.back();
            mAggregateCpmDynamics.position = cpmDynamics.trackedDynamics.position;
            mAggregateCpmDynamics.speed = cpmDynamics.trackedDynamics.speed;
        }
    }

    return std::make_pair(cpmDynamics, entry.second);
}

size_t RemoteDynamics::getCpmDynamicsCount() const
{
    size_t result = 0;

    for (const auto& entry : mCpmDynamics) { result += entry.second.size(); }

    return result;
}


boost::optional<CpmDynamics> RemoteDynamics::getLatestCpmDynamics() const
{
    boost::optional<CpmDynamics> latest;
    if (mHistoryCpmDynamics) {
        latest = CpmDynamics{
            mHistoryCpmDynamics->received, mHistoryCpmDynamics->senderPosition,
            // The correct timestamp and heading are not present
            TrackedDynamics{mHistoryCpmDynamics->received, mAggregateCpmDynamics.position, mAggregateCpmDynamics.speed, vanetza::units::Angle()}};
    }

    return latest;
}


void RemoteDynamics::clearExpired(const omnetpp::SimTime& now)
{
    static const opp::SimTime lifetime(1100, opp::SIMTIME_MS);

    for (auto station = mCpmDynamics.begin(); station != mCpmDynamics.end();) {
        // The CPM's are ordered by time, oldest element first
        auto& cpmDynamics = station->second;
        while (!cpmDynamics.empty()) {
            if (cpmDynamics.front().received + lifetime <= now) {
                cpmDynamics.pop_front();
            } else {
                break;
            }
        }
        if (cpmDynamics.empty()) {
            station = mCpmDynamics.erase(station);
        } else {
            ++station;
        }
    }

    if (mCpmDynamics.empty()) {
        mHistoryCpmDynamics = nullptr;
    }
}


std::pair<boost::circular_buffer<HistoryCpmDynamics>&, RemoteDynamics::CpmResult> RemoteDynamics::getHistoryCpmDynamicsEntry(
    StationID_t stationId, const omnetpp::SimTime& timestamp, CpmOption option)
{
#if 1
    auto cpmDynamics = mCpmDynamics.find(stationId);
    if (cpmDynamics == mCpmDynamics.end()) {
        cpmDynamics = mCpmDynamics.emplace(stationId, 12).first;
    }

    if (!cpmDynamics->second.empty()) {
        if (cpmDynamics->second.back().received == timestamp) {
            if (option == CpmOption::DuplicateReplace) {
                cpmDynamics->second.pop_back();

                return std::make_pair(std::ref(cpmDynamics->second), CpmResult::DuplicateReplace);
            } else if (option == CpmOption::DuplicateDrop) {
                return std::make_pair(std::ref(cpmDynamics->second), CpmResult::DuplicateDrop);
            }
        }
        if (cpmDynamics->second.full()) {
            cpmDynamics->second.pop_front();

            return std::make_pair(std::ref(cpmDynamics->second), CpmResult::HistoryReplace);
        }
    }

    return std::make_pair(std::ref(cpmDynamics->second), CpmResult::Ok);
#else
    // For performance comparison with IBR limit to one entry
    // Requires elements get added with increasing reception time or history pointer gets wrong value
    auto cpmDynamics = mCpmDynamics.find(stationId);
    if (cpmDynamics == mCpmDynamics.end()) {
        cpmDynamics = mCpmDynamics.emplace(stationId, 1).first;
    }

    if (cpmDynamics->second.full()) {
        cpmDynamics->second.pop_front();
        mHistoryCpmDynamics = nullptr;
    }

    return std::make_pair(std::ref(cpmDynamics), CpmResult::Ok);
#endif
}


std::pair<omnetpp::SimTime, omnetpp::SimTime> aggregateTrackingTime(
    const artery::LocalEnvironmentModel::Tracking::TrackingMap& tracking, const std::string& sensorCategory)
{
    // Apparently SimTime runs only between ZERO and getMaxTime()
    auto result = std::make_pair(opp::SimTime::getMaxTime(), opp::SimTime::ZERO);

    for (const auto& entry : tracking) {
        if (entry.first->getSensorCategory() != sensorCategory) {
            continue;
        }
        if (entry.second.first() < result.first) {
            result.first = entry.second.first();
        }
        if (entry.second.last() > result.second) {
            result.second = entry.second.last();
        }
    }

    return result;
}

}  // namespace cp
}  // namespace artery
