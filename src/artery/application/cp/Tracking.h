/*
 * Artery V2X Simulation Framework
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_CP_TRACKING_H_
#define ARTERY_CP_TRACKING_H_

/**
 * @file Tracking.h
 * @brief Data types and functions to extend LocalEnvironmentModel data with required information to create CPM's
 */

#include "artery/envmod/LocalEnvironmentModel.h"
#include "artery/utility/Geometry.h"

#include <boost/circular_buffer.hpp>
#include <boost/optional.hpp>
#include <omnetpp/simtime.h>
#include <vanetza/asn1/its/StationID.h>
#include <vanetza/units/angle.hpp>
#include <vanetza/units/velocity.hpp>

#include <map>
#include <memory>
#include <utility>
#include <vector>


namespace vanetza
{
namespace asn1
{
class Cam;
}  // namespace asn1
}  // namespace vanetza


namespace artery
{
namespace cp
{

/**
 * @brief Dynamic properties of a tracked object
 *
 * This is a subset of the dynamic data provided by VehicleDataProvider required to create CPM's.
 */
struct TrackedDynamics {
    /**
     * @brief Timestamp of this data set
     */
    omnetpp::SimTime timestamp;
    /**
     * @brief Object position in OMNeT++ coordinates
     */
    artery::Position position;
    /**
     * @brief Object movement speed
     */
    vanetza::units::Velocity speed;
    /**
     * @brief Object movement heading
     */
    vanetza::units::Angle heading;
};

/**
 * @brief All additional properties of a local tracked object
 */
class LocalDynamics
{
public:
    /**
     * @brief Construct a new Local Dynamics object
     *
     * Initialize previous dynamic properties as not available.
     */
    LocalDynamics() { clearPrevDynamics(); }

    /**
     * @brief Returns if this object was included in a CPM
     *
     * @return True if this object was included in a CPM
     */
    bool hasPrevDynamics() const { return (mPrevDynamics.timestamp >= omnetpp::SimTime::ZERO); }
    /**
     * @brief Get the dynamic properties of the last time this object was included in a CPM
     *
     * @return The dynamic properties of the object the last time it was included in a CPM, unspecified if hasPrevDynamics() returns false
     */
    const TrackedDynamics& getPrevDynamics() const { return mPrevDynamics; }
    /**
     * @brief Set the dynamic properties of the last time this object was included in a CPM
     *
     * @param prevDynamics The dynamic properties of the object the last time it was included in a CPM
     */
    void setPrevDynamics(const TrackedDynamics& prevDynamics) { mPrevDynamics = prevDynamics; }
    /**
     * @brief Removes the dynamic properties of the object the last time it was included in a CPM
     *
     * After this method call hasPrevDynamics() returns false;
     */
    void clearPrevDynamics() { mPrevDynamics.timestamp.setRaw(-1); }

private:
    /**
     * @brief Dynamic properties of the last time the object was included in a CPM
     *
     * The timestamp is the inclusion time, a negative timestamp is interpreted as no value available.
     */
    TrackedDynamics mPrevDynamics;
};

/**
 * @brief All local tracked objects
 *
 * This map is a supplement to LocalEnvironmentModel data using the same key element and ordering.
 */
using LocalTracking = std::map<artery::LocalEnvironmentModel::Object, LocalDynamics, std::owner_less<artery::LocalEnvironmentModel::Object>>;
/**
 * @brief A local tracked object
 */
using LocalObject = typename LocalTracking::value_type;


/**
 * @brief Dynamic properties of an object received by CPM
 */
struct CpmDynamics {
    /**
     * @brief Reception timestamp
     */
    omnetpp::SimTime received;
    /**
     * @brief Position of the sender in OMNeT++ coordinates
     */
    artery::Position senderPosition;
    /**
     * @brief Dynamic properties
     *
     * The contained timestamp is the measurement timestamp of the sender.
     */
    TrackedDynamics trackedDynamics;
};

/**
 * @brief History dynamic properties of an object received by CPM
 *
 * This information is stored for each received CPM.
 */
struct HistoryCpmDynamics {
    /**
     * @brief Reception timestamp
     */
    omnetpp::SimTime received;
    /**
     * @brief Position of the sender in OMNeT++ coordinates
     */
    artery::Position senderPosition;
};

/**
 * @brief Aggregate dynamic properties of an object received by CPM
 *
 * This information is aggregated over all received CPM's.
 */
struct AggregateCpmDynamics {
    /**
     * @brief Object position in OMNeT++ coordinates
     */
    artery::Position position;
    /**
     * @brief Object movement speed
     */
    vanetza::units::Velocity speed;
};

/**
 * @brief All additional properties of a remote tracked object
 */
class RemoteDynamics
{
public:
    /**
     * @brief CPM insertion options
     */
    enum class CpmOption {
        /**
         * @brief No special processing
         */
        None,
        /**
         * @brief Replace duplicate entry
         */
        DuplicateReplace,
        /**
         * @brief Drop duplicate entry
         */
        DuplicateDrop,
    };
    /**
     * @brief CPM insertion result
     */
    enum class CpmResult {
        /**
         * @brief Insertion successful
         */
        Ok,
        /**
         * @brief Insertion successful but oldest history element got removed
         */
        HistoryReplace,
        /**
         * @brief Insertion successful but previous duplicate got replaced
         */
        DuplicateReplace,
        /**
         * @brief Insertion failed because of duplicate entry
         */
        DuplicateDrop,
    };

    /**
     * @brief CpmDynamics of each sending station
     *
     * For each sender the dynamic properties of each received CPM for this object.
     * Each entry in the buffer corresponds to one received CPM,
     * this assumes that a CPM does contain unique objects only.
     *
     * The dynamic properties are ordered by reception time, old elements first.
     */
    using StationCpmDynamics = std::map<StationID_t, boost::circular_buffer<HistoryCpmDynamics>>;

    /**
     * @brief Construct a new Remote Dynamics object
     */
    RemoteDynamics();
    /**
     * @brief Construct a new Remote Dynamics object
     *
     * @param cam CAM object to set
     */
    explicit RemoteDynamics(const vanetza::asn1::Cam* cam);

    /**
     * @brief Set the latest received CAM
     *
     * @param cam CAM object
     */
    void setCam(const vanetza::asn1::Cam* cam) { mCam = cam; }
    /**
     * @brief Get the latest received CAM
     *
     * @return CAM object, nullptr if not available
     */
    const vanetza::asn1::Cam* getCam() const { return mCam; }

    /**
     * @brief Add CPM Dynamics for a sender
     *
     * @param stationId Station ID of sender
     * @param cpmDynamics CPM Dynamics of object
     * @param option Insertion option
     * @return Pair of inserted CPM Dynamics and insertion result.
     */
    std::pair<CpmDynamics, CpmResult> addCpmDynamics(StationID_t stationId, const CpmDynamics& cpmDynamics, CpmOption option = CpmOption::None);
    /**
     * @brief Get the CPM Dynamics of all senders
     *
     * @return CPM dynamics of all senders
     */
    const StationCpmDynamics& getCpmDynamics() const { return mCpmDynamics; }
    /**
     * @brief Returns if CPM Dynamics are available
     *
     * @return True, if CPM Dynamics are available
     */
    bool hasCpmDynamics() const { return !mCpmDynamics.empty(); }
    /**
     * @brief Get the count of all CPM Dynamics
     *
     * @return The count of all CPM Dynamics over all senders
     */
    size_t getCpmDynamicsCount() const;

    /**
     * @brief Get the latest CPM dynamics over all senders
     *
     * Implementation note: For memory footprint reasons the TrackedDynamics timestamp and heading are invalid
     *                      because this information doesn't get stored
     *
     * @return Lastest CPM dynamics over all senders, boost::none if hasCpmDynamics() returns false
     */
    boost::optional<CpmDynamics> getLatestCpmDynamics() const;
    /**
     * @brief Get the latest history CPM dynamics over all senders
     *
     * This method is more performant than getLatestCpmDynamics().
     *
     * @return Lastest history CPM dynamics over all senders, nullptr if hasCpmDynamics() returns false
     */
    const HistoryCpmDynamics* getLatestHistoryCpmDynamics() const { return mHistoryCpmDynamics; }
    /**
     * @brief Get the latest aggregate CPM dynamics over all senders
     *
     * This method is more performant than getLatestCpmDynamics().
     *
     * @return Lastest aggregate CPM dynamics over all senders, undefined if hasCpmDynamics() returns false
     */
    const AggregateCpmDynamics& getLatestAggregateCpmDynamics() const { return mAggregateCpmDynamics; }

    /**
     * @brief Remove all expired entries
     *
     * @param now Current timestamp
     */
    void clearExpired(const omnetpp::SimTime& now);
    /**
     * @brief Check if this object is expired
     *
     * @return true This object is expired, no data present
     * @return false This object is not expired, some data might not be present
     */
    bool isExpired() const { return (!mCam && mCpmDynamics.empty()); }

private:
    /**
     * @brief Get the history CPM Dynamics object of the sender
     *
     * Initializes the buffer if necessary and performes the necessary operations required by the insertion option.
     * If CpmResult::DuplicateDrop gets returned the next element must not be inserted.
     *
     * @param stationId StationID of sender
     * @param timestamp Timestamp of next element to insert
     * @param option Insertion option
     * @return Pair of history CPM Dynamics of sender and insertion result.
     */
    std::pair<boost::circular_buffer<HistoryCpmDynamics>&, CpmResult> getHistoryCpmDynamicsEntry(
        StationID_t stationId, const omnetpp::SimTime& timestamp, CpmOption option);

private:
    /**
     * @brief Latest received CAM
     */
    const vanetza::asn1::Cam* mCam;

    /**
     * @brief Dynamic properties received by CPM
     */
    StationCpmDynamics mCpmDynamics;
    /**
     * @brief The latest historic dynamic properties across all senders, nullptr if none available
     *
     * This is a pointer into mCpmDynamics, care must be taken to update this pointer accordingly.
     *
     * TODO: Is it possible to receive two CPM's at exactly the same timestamp?
     *       If this can happen, how can the latest CPM be determined in a deterministic way?
     */
    HistoryCpmDynamics* mHistoryCpmDynamics;
    /**
     * @brief The latest aggregated dynamic properties accross all senders, undefined if mHistoryCpmDynamics == nullptr
     */
    AggregateCpmDynamics mAggregateCpmDynamics;
};

/**
 * @brief All remote tracked objects
 *
 * This includes all sinks that receive information about objects from remote sources,
 * currently this is the CaService and CpService.
 *
 * This map is not related to LocalEnvironmentModel, it is itself the source of information.
 * The received CPM's do only contain StationID's, the lookup of the corresponding LocalEnvironmentModel objects
 * requires two expensive lookups which are also performed by CpmSensor. Since the LocalEnvironmentModel does
 * not offer any extra data beside the first tracking time (which could be easily stored here as well)
 * and the feature to expire entries (which is also done here as well) the extra work to synchronize
 * both models is saved.
 */
using RemoteTracking = std::map<StationID_t, RemoteDynamics>;

/**
 * @brief A remote tracked object
 */
using RemoteObject = typename RemoteTracking::value_type;


/**
 * @brief Candidates to be included into Perceived Object Container
 *
 * Contains pointers to local tracked objects.
 */
using PocCandidates = std::vector<LocalObject*>;


/**
 * @brief Determine the first and last tracking time over all sensors of a specific category
 *
 * @param tracking Tracking map to process
 * @param sensorCategory Sensor category to use
 * @return Aggregated first and last tracking time
 */
std::pair<omnetpp::SimTime, omnetpp::SimTime> aggregateTrackingTime(
    const artery::LocalEnvironmentModel::Tracking::TrackingMap& tracking, const std::string& sensorCategory);

}  // namespace cp
}  // namespace artery

#endif /* ARTERY_CP_TRACKING_H_ */
