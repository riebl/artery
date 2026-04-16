/*
 * Artery V2X Simulation Framework
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_CPSERVICE_H_
#define ARTERY_CPSERVICE_H_

#include "artery/application/ItsG5BaseService.h"
#include "artery/envmod/LocalEnvironmentModel.h"
#include "artery/utility/Channel.h"
#include "artery/utility/Geometry.h"

#include <omnetpp/simtime.h>
#include <vanetza/asn1/cpm.hpp>
#include <vanetza/btp/data_interface.hpp>
#include <vanetza/units/angle.hpp>
#include <vanetza/units/velocity.hpp>

#include <cstdint>
#include <limits>
#include <optional>
#include <unordered_map>
#include <vector>

namespace artery
{

using Cpm = vanetza::asn1::r2::Cpm;

class NetworkInterfaceTable;
class Timer;
class VehicleDataProvider;

class CpService : public ItsG5BaseService
{
public:
    struct VdpSnapshot {
        omnetpp::SimTime updated;
        uint32_t stationId;
        int stationType;  // vanetza::geonet::StationType as int
        Position position;
        long referenceLatitude;        // same encoding as mc.referencePosition.latitude
        long referenceLongitude;       // same encoding as mc.referencePosition.longitude
        long orientationAngleDeciDeg;  // same encoding as ovc.orientationAngle.value
    };

    struct SensorSnapshot {
        const Sensor* sensor = nullptr;
        uint8_t id = 0;
        long type = 0;  // Vanetza_ITS2_SensorType_t (typedef long)
        bool shadowingApplies = false;
    };

    struct ObjectVdpSnapshot {
        bool hasVdpData = false;
        vanetza::units::Velocity speed = 0.0 * vanetza::units::si::meter_per_second;
        vanetza::units::Angle heading = vanetza::units::Angle(0.0 * vanetza::units::degree);
        vanetza::geonet::StationType stationType = vanetza::geonet::StationType::Unknown;
    };

    using ObjectVdpSnapshotMap = std::unordered_map<uint32_t, ObjectVdpSnapshot>;

    enum class PerceivedObjectType : std::uint8_t { Unknown = 0, TypeA, TypeB };

    struct PerceivedObjectSnapshot {
        uint32_t lemId = 0;
        uint16_t cpsId = 0;

        // relative position in centimeters (x/y)
        long xCm = 0;
        long yCm = 0;

        long measurementDeltaTimeMs = 0;  // clamped to ASN.1 range (-2048..2047)
        int64_t objectAgeMs = 0;          // clamped to ASN.1 range (0..2047)

        long objectPerceptionQuality = 0;  // Vanetza_ITS2_ObjectPerceptionQuality_t (typedef long)

        bool hasVelocity = false;
        double speedMps = 0.0;
        double headingDeg = 0.0;

        PerceivedObjectType type = PerceivedObjectType::Unknown;
        int stationType = -1;  // vanetza::geonet::StationType as int, -1 = unavailable

        std::vector<uint8_t> sensorIds;  // contributing CPS sensorIds (Identifier1B)
        double utility = 0.0;

        // cached values for inclusion and utility calculation
        bool haveHistory = false;
        double distanceDiffM = 0.0;
        double speedDiffMps = 0.0;
        double orientationDiffDeg = 0.0;
        double sinceLastInclusionSeconds = 0.0;
    };

    struct PerceivedObjectHistory {
        omnetpp::SimTime inclusion;
        Position position;
        double speed;
        Angle orientation;
    };

    CpService();
    void initialize() override;
    void indicate(const vanetza::btp::DataIndication&, std::unique_ptr<vanetza::UpPacket>) override;
    void trigger() override;

private:
    void checkTriggeringConditions(const omnetpp::SimTime& T_now);
    void sendCpm(const omnetpp::SimTime& T_now);
    void captureVdpSnapshot();
    void captureSensorSnapshot(const omnetpp::SimTime& T_now, const std::vector<Sensor*>& sensors);
    ObjectVdpSnapshotMap captureObjectVdpSnapshot(const LocalEnvironmentModel::TrackedObjects& objects) const;
    void capturePerceivedObjectSnapshot(
        const omnetpp::SimTime& T_now, uint64_t referenceTime, const LocalEnvironmentModel::TrackedObjects& objects,
        const ObjectVdpSnapshotMap& objectVdpSnapshots);
    bool checkSensorInformationTrigger(const omnetpp::SimTime& T_now);
    bool checkPerceptionRegionTrigger();
    bool checkPerceivedObjectTrigger(const omnetpp::SimTime& T_now);
    double calculateUtilityFunction(
        const PerceivedObjectSnapshot& po, double distanceDiff, double speedDiff, double orientationDiff, double lastInclusionSeconds);
    void sortPerceivedObjects();
    omnetpp::SimTime genCpmDcc();
    void handlePseudonymChange();
    std::optional<uint16_t> allocateCpmObjectId(const omnetpp::SimTime& T_now, uint32_t lemId, int64_t objectAgeMs);
    std::optional<uint8_t> allocateCpmSensorId(const omnetpp::SimTime& T_now, const int sensorId);
    PerceivedObjectType toPOType(vanetza::geonet::StationType st);

    ChannelNumber mPrimaryChannel = channel::CCH;
    const NetworkInterfaceTable* mNetworkInterfaceTable = nullptr;
    const VehicleDataProvider* mVehicleDataProvider = nullptr;
    const Timer* mTimer = nullptr;
    LocalDynamicMap* mLocalDynamicMap = nullptr;
    LocalEnvironmentModel* mLocalEnvironmentModel = nullptr;
    VdpSnapshot mVdpSnapshot;

    omnetpp::SimTime mGenCpmMin;
    omnetpp::SimTime mGenCpmMax;
    omnetpp::SimTime mGenCpm;
    omnetpp::SimTime mLastCpmTimestamp;
    bool mDccRestriction;
    omnetpp::SimTime mAddSensorInformation;
    omnetpp::SimTime mLastSensorInformationTimestamp;
    int mObjectInclusionConfig;
    double mObjectPerceptionQualityThreshold;
    vanetza::units::Length mMinPositionChangeThreshold;
    vanetza::units::Velocity mMinGroundSpeedChangeThreshold;
    vanetza::units::Angle mMinGroundVelocityOrientationChangeThreshold;
    std::unordered_map<int, PerceivedObjectHistory> mPerceivedObjectHistories;  // map of object ID to its history

    vanetza::units::Length mMinPositionChangePriorityThreshold;
    vanetza::units::Length mMaxPositionChangePriorityThreshold;
    vanetza::units::Velocity mMinGroundSpeedChangePriorityThreshold;
    vanetza::units::Velocity mMaxGroundSpeedChangePriorityThreshold;
    vanetza::units::Angle mMinGroundVelocityOrientationChangePriorityThreshold;
    vanetza::units::Angle mMaxGroundVelocityOrientationChangePriorityThreshold;
    omnetpp::SimTime mMinLastInclusionTimePriorityThreshold;
    omnetpp::SimTime mMaxLastInclusionTimePriorityThreshold;
    std::vector<std::size_t> mSelectedCpmObjects;  // indices into mPerceivedObjectSnapshot

    // CPM object ID allocation. ETSI TS 103 324 (0..65535, reuse holdoff, reset on pseudonym change)
    omnetpp::SimTime mUnusedObjectIdRetentionPeriod;
    bool mHaveStationId;
    uint32_t mLastStationId;
    std::unordered_map<uint32_t, uint16_t> mLem2Cps;
    std::vector<uint32_t> mCps2Lem;
    std::vector<omnetpp::SimTime> mCpmObjectIdLastUsed;
    std::vector<int64_t> mCpmObjectIdLastAgeMs;
    std::vector<PerceivedObjectSnapshot> mPerceivedObjectSnapshot;

    // CPM sensorId allocation. ETSI TS 103 324 (0..255, stable mapping, reuse holdoff, reset on pseudonym change)
    omnetpp::SimTime mUnusedSensorIdRetentionPeriod;
    std::unordered_map<int, uint8_t> mSensorId2Cps;
    std::vector<int> mCps2SensorId;  // cpsId -> LEM sensorId, kInvalidLemSensorId if not in use
    std::vector<omnetpp::SimTime> mCpmSensorIdLastUsed;
    std::vector<SensorSnapshot> mSensorSnapshot;
};

Cpm createCollectivePerceptionMessage(const CpService::VdpSnapshot& vdp, uint64_t referenceTime);
void addOriginatingVehicleContainer(Cpm& message, const CpService::VdpSnapshot& vdp);
void addOriginatingRsuContainer(Cpm& message);
void addSensorInformationContainer(Cpm& message, const std::vector<CpService::SensorSnapshot>& sensorSnapshot);
void addPerceptionRegionContainer(Cpm& message);
void addPerceivedObjectContainer(
    Cpm& message, const std::vector<CpService::PerceivedObjectSnapshot>& perceivedObjectSnapshot, const std::vector<std::size_t>& selectedObjects,
    const omnetpp::SimTime& lastCpmTimestamp);


}  // namespace artery

#endif /* ARTERY_CPSERVICE_H_ */
