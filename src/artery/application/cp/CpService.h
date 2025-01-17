/*
 * Artery V2X Simulation Framework
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_CPSERVICE_H_
#define ARTERY_CPSERVICE_H_

/**
 * @file CpService.h
 * @brief Implementation of Collective Perception Service
 */

#include "artery/application/ItsG5Service.h"
#include "artery/application/cp/Tracking.h"
#include "artery/application/cp/Utility.h"
#include "artery/utility/Channel.h"

#include <boost/optional.hpp>
#include <omnetpp/simtime.h>
#include <vanetza/asn1/cpm.hpp>
#include <vanetza/dcc/profile.hpp>

#include <string>
#include <vector>


namespace artery
{
class LocalDynamicMap;
class LocalEnvironmentModel;
class NetworkInterfaceTable;
class Timer;
class VehicleDataProvider;

namespace cp
{
class GenerationRule;
class RedundancyMitigationRule;
}  // namespace cp
}  // namespace artery

namespace traci
{
class VehicleController;
}  // namespace traci


namespace artery
{

/**
 * @brief Collective Perception Service Core implementation based on ETSI TR 103 562 V2.1.1
 */
class CpService : public ItsG5Service
{
public:
    CpService();
    ~CpService() override;

protected:
    int numInitStages() const override;
    void initialize(int stage) override;
    void finish() override;

public:
    void indicate(const vanetza::btp::DataIndication& ind, std::unique_ptr<vanetza::UpPacket> packet, const NetworkInterface& interface) override;
    void trigger() override;

private:
    /**
     * @brief Check if a CPM must be created
     *
     * Updates the generation interval to the predicted next generation interval.
     *
     * @param now Timestamp to test
     * @return True, if a CPM must be created at the given timestamp
     */
    bool checkTriggeringConditions(const omnetpp::SimTime& now);
    /**
     * @brief Check if a Sensor Information container must be created
     *
     * @param now Timestamp to test
     * @return True, if a CPM must be created at the given timestamp
     */
    bool checkSensorInfoConditions(const omnetpp::SimTime& now);
    /**
     * @brief Synchronize local tracking data
     *
     * Compares local tracking data against the LEM, removes expired entries and adds new entries.
     *
     * @param now Current timestamp
     */
    void synchronizeLocalTracking(const omnetpp::SimTime& now);
    /**
     * @brief Synchronize remote tracking data
     *
     * Compares remote tracking data against external sources and removes expired entries.
     *
     * @param now Current timestamp
     */
    void synchronizeRemoteTracking(const omnetpp::SimTime& now);
    /**
     * @brief Generate and transmit a Collective Perception Message with the supplied data
     *
     * This may result in multiple CPM's if segmentation must be used.
     *
     * @param now Generation Timestamp
     * @param pocCandidates Objects to include
     * @param sic Sensor Information Container to include, may be nullptr
     */
    void generateCollectivePerceptionMessage(const omnetpp::SimTime& now, cp::PocCandidates& pocCandidates, cp::SensorInformationContainer_ptr sic);

    /**
     * @brief Updates local tracking data of objects that have been sent as CPM
     *
     * @param now Generation Timestamp of the CPM
     * @param pocCandidates Objects that have been sent
     */
    void updateLocalTracking(const omnetpp::SimTime& now, cp::PocCandidates& pocCandidates);

    /**
     * @brief Create a basic Collective Perception Message
     *
     * This message contains only the required containers (Management and Station Data)
     * which are filled with the current vehicle data.
     *
     * @param numberOfPerceivedObjects The number of perceived objects
     * @param segmented If true, the optional Perceived Object Container Segment Info gets also created, initialized for a single CPM
     * @return The Collective Perception Message
     */
    vanetza::asn1::Cpm createCollectivePerceptionMessage(NumberOfPerceivedObjects_t numberOfPerceivedObjects, bool segmented = false);
    /**
     * @brief Create a Sensor Information Container
     *
     * @return The Sensor Information Container
     */
    cp::SensorInformationContainer_ptr createSensorInformationContainer();
    /**
     * @brief Create Perceived Objects
     *
     * @param pocCandidates Perceived Object candidates to process
     * @return Perceived Objects created from the candidates
     */
    cp::PerceivedObjects createPerceivedObjects(const cp::PocCandidates& pocCandidates);

    /**
     * @brief Sends the CPM over the network interface
     *
     * @param cpm CPM to send, will be moved from
     */
    void transmitCpm(vanetza::asn1::Cpm&& cpm);

    /**
     * @brief Get the current CBR value of the used primary channel
     *
     * @return Current CRB value
     */
    double getCurrentCbr() const;
    /**
     * @brief Generate the next CPM generation timestamp using DCC generation rules
     *
     * This uses the generation rules of the CaService.
     *
     * @return Next CPM generation timestamp
     */
    omnetpp::SimTime generateCpmDcc();

    /**
     * @brief Emit statistics about the changes of a remote object after it got updated
     *
     * @param object Tracking data of the remote object
     * @param prevDynamics CPM Dynamics before the update
     * @param currentDynamics CPM Dynamics after the update
     */
    void emitRemoteObjectStatistics(
        const cp::RemoteDynamics& object, const boost::optional<cp::CpmDynamics> prevDynamics, const cp::CpmDynamics& currentDynamics);

private:
    /**
     * @brief Generation Rule to use
     */
    cp::GenerationRule* mGenerationRule;
    /**
     * @brief Redundancy Mitigation Rules to use
     *
     * The rules get applied in element order of the vector.
     */
    std::vector<cp::RedundancyMitigationRule*> mRedundancyMitigationRules;

    /**
     * @brief Network channel to send CPM's on
     */
    ChannelNumber mPrimaryChannel;
    /**
     * @brief Network interfaces of the Middleware
     */
    const NetworkInterfaceTable* mNetworkInterfaceTable;
    /**
     * @brief Vehicle data of the associated vehicle
     */
    const VehicleDataProvider* mVehicleDataProvider;
    /**
     * @brief TraCI controller of the associated vehicle
     */
    const traci::VehicleController* mVehicleController;
    /**
     * @brief Timer to convert simulation timestamps
     */
    const Timer* mTimer;
    /**
     * @brief Local known objects from all sensors
     */
    const LocalEnvironmentModel* mLocalEnvironmentModel;
    /**
     * @brief Local known objects from CaService
     */
    const LocalDynamicMap* mLocalDynamicMap;

    // *** NED defined parameters ***
    omnetpp::SimTime mGenCpmMin;
    omnetpp::SimTime mGenCpmMax;
    omnetpp::SimTime mAddSensorInformation;

    int mMtuCpm;
    vanetza::dcc::Profile mDccProfile;
    bool mWithDccRestriction;

    bool mDynamicRedundancyMitigation;
    double mRedundancyLoad;
    // ***

    /**
     * @brief Interval when the next CPM must be generated
     */
    omnetpp::SimTime mGenCpm;
    /**
     * @brief Timestamp of the last generated CPM
     */
    omnetpp::SimTime mLastCpm;
    /**
     * @brief Timestamp of the last generated Sensor Information Container
     */
    omnetpp::SimTime mLastSensorInfoContainer;

    /**
     * @brief Sensor category to use for local tracking
     *
     * Currently there exists only one type of sensors, radar sensors, and the LEM
     * can only be filtered by one category.
     */
    std::string mLocalTrackingCategory;
    /**
     * @brief Additional local tracking data
     *
     * This supplements the data of the LEM.
     */
    cp::LocalTracking mLocalTracking;
    /**
     * @brief Remote tracking data
     *
     * This data is extracted from received CPM's and completed with data from received CAM's.
     */
    cp::RemoteTracking mRemoteTracking;
};

}  // namespace artery

#endif /* ARTERY_CPSERVICE_H_ */
