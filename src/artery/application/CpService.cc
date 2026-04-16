/*
 * Artery V2X Simulation Framework
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/CpService.h"

#include "artery/application/Asn1PacketVisitor.h"
#include "artery/application/CpObject.h"
#include "artery/application/MultiChannelPolicy.h"
#include "artery/application/VehicleDataProvider.h"
#include "artery/envmod/TraCIEnvironmentModelObject.h"
#include "artery/envmod/sensor/FovSensor.h"
#include "artery/utility/round.h"
#include "artery/utility/simtime_cast.h"
#include "veins/base/utils/Coord.h"

#include <boost/units/cmath.hpp>
#include <boost/units/systems/si/prefixes.hpp>
#include <omnetpp/cexception.h>
#include <vanetza/btp/ports.hpp>
#include <vanetza/dcc/transmission.hpp>
#include <vanetza/dcc/transmit_rate_control.hpp>
#include <vanetza/geonet/station_type.hpp>

#include <algorithm>
#include <chrono>

namespace artery
{

using namespace omnetpp;

static const simsignal_t scSignalCpmReceived = cComponent::registerSignal("CpmReceived");
static const simsignal_t scSignalCpmSent = cComponent::registerSignal("CpmSent");

// CPM object/sensor ID allocation constants
static constexpr std::size_t kCpmObjectIdSpace = 65536;
static constexpr uint32_t kInvalidLemId = std::numeric_limits<uint32_t>::max();
static constexpr std::size_t kCpmSensorIdSpace = 256;
static constexpr int kInvalidLemSensorId = -1;
static constexpr const_simtime_t kNeverUsedSimTime = -1;

void setPOClassification(Vanetza_ITS2_PerceivedObject_t& po, vanetza::geonet::StationType st)
{
    po.classification = vanetza::asn1::allocate<Vanetza_ITS2_ObjectClassDescription_t>();

    auto* occ = vanetza::asn1::allocate<Vanetza_ITS2_ObjectClassWithConfidence_t>();

    occ->confidence = Vanetza_ITS2_ConfidenceLevel_unavailable;

    using vanetza::geonet::StationType;

    switch (st) {
        // VRUs
        case StationType::Pedestrian:
            occ->objectClass.present = Vanetza_ITS2_ObjectClass_PR_vruSubClass;
            occ->objectClass.choice.vruSubClass.present = Vanetza_ITS2_VruProfileAndSubprofile_PR_pedestrian;
            occ->objectClass.choice.vruSubClass.choice.pedestrian = Vanetza_ITS2_VruSubProfilePedestrian_ordinary_pedestrian;
            break;

        case StationType::Cyclist:
            occ->objectClass.present = Vanetza_ITS2_ObjectClass_PR_vruSubClass;
            occ->objectClass.choice.vruSubClass.present = Vanetza_ITS2_VruProfileAndSubprofile_PR_bicyclistAndLightVruVehicle;
            occ->objectClass.choice.vruSubClass.choice.bicyclistAndLightVruVehicle = Vanetza_ITS2_VruSubProfileBicyclist_bicyclist;
            break;

        case StationType::Moped:
            occ->objectClass.present = Vanetza_ITS2_ObjectClass_PR_vruSubClass;
            occ->objectClass.choice.vruSubClass.present = Vanetza_ITS2_VruProfileAndSubprofile_PR_motorcyclist;
            occ->objectClass.choice.vruSubClass.choice.motorcyclist = Vanetza_ITS2_VruSubProfileMotorcyclist_moped;
            break;

        case StationType::Motorcycle:
            occ->objectClass.present = Vanetza_ITS2_ObjectClass_PR_vruSubClass;
            occ->objectClass.choice.vruSubClass.present = Vanetza_ITS2_VruProfileAndSubprofile_PR_motorcyclist;
            occ->objectClass.choice.vruSubClass.choice.motorcyclist = Vanetza_ITS2_VruSubProfileMotorcyclist_motorcycle;
            break;

        // vehicles
        case StationType::Passenger_Car:
            occ->objectClass.present = Vanetza_ITS2_ObjectClass_PR_vehicleSubClass;
            occ->objectClass.choice.vehicleSubClass = Vanetza_ITS2_TrafficParticipantType_passengerCar;
            break;

        case StationType::Bus:
            occ->objectClass.present = Vanetza_ITS2_ObjectClass_PR_vehicleSubClass;
            occ->objectClass.choice.vehicleSubClass = Vanetza_ITS2_TrafficParticipantType_bus;
            break;

        case StationType::Light_Truck:
            occ->objectClass.present = Vanetza_ITS2_ObjectClass_PR_vehicleSubClass;
            occ->objectClass.choice.vehicleSubClass = Vanetza_ITS2_TrafficParticipantType_lightTruck;
            break;

        case StationType::Heavy_Truck:
            occ->objectClass.present = Vanetza_ITS2_ObjectClass_PR_vehicleSubClass;
            occ->objectClass.choice.vehicleSubClass = Vanetza_ITS2_TrafficParticipantType_heavyTruck;
            break;

        case StationType::Trailer:
            occ->objectClass.present = Vanetza_ITS2_ObjectClass_PR_vehicleSubClass;
            occ->objectClass.choice.vehicleSubClass = Vanetza_ITS2_TrafficParticipantType_trailer;
            break;

        case StationType::Special_Vehicle:
            occ->objectClass.present = Vanetza_ITS2_ObjectClass_PR_vehicleSubClass;
            occ->objectClass.choice.vehicleSubClass = Vanetza_ITS2_TrafficParticipantType_specialVehicle;
            break;

        case StationType::Tram:
            occ->objectClass.present = Vanetza_ITS2_ObjectClass_PR_vehicleSubClass;
            occ->objectClass.choice.vehicleSubClass = Vanetza_ITS2_TrafficParticipantType_tram;
            break;

        case StationType::RSU:
            occ->objectClass.present = Vanetza_ITS2_ObjectClass_PR_vehicleSubClass;
            occ->objectClass.choice.vehicleSubClass = Vanetza_ITS2_TrafficParticipantType_roadSideUnit;
            break;

        default:  // if unknown, drop classification entirely
            ASN_STRUCT_FREE(asn_DEF_Vanetza_ITS2_ObjectClassWithConfidence, occ);
            ASN_STRUCT_FREE(asn_DEF_Vanetza_ITS2_ObjectClassDescription, po.classification);
            po.classification = nullptr;
            return;
    }

    int ret = ASN_SEQUENCE_ADD(&po.classification->list, occ);
    assert(ret == 0);
}


Define_Module(CpService)

CpService::CpService() : mGenCpmMin{100, SIMTIME_MS}, mGenCpmMax{1000, SIMTIME_MS}, mGenCpm(mGenCpmMax)
{
}

void CpService::initialize()
{
    ItsG5BaseService::initialize();
    mNetworkInterfaceTable = getFacilities().get_const_ptr<NetworkInterfaceTable>();
    mVehicleDataProvider = getFacilities().get_const_ptr<VehicleDataProvider>();
    mTimer = getFacilities().get_const_ptr<Timer>();
    mLocalDynamicMap = getFacilities().get_mutable_ptr<artery::LocalDynamicMap>();
    mLocalEnvironmentModel = getFacilities().get_mutable_ptr<LocalEnvironmentModel>();

    // avoid unreasonable high elapsed time values for newly inserted vehicles
    mLastCpmTimestamp = simTime();

    // generation rate boundaries
    mGenCpmMin = par("minInterval");
    mGenCpmMax = par("maxInterval");
    mGenCpm = mGenCpmMax;

    mAddSensorInformation = par("addSensorInformation");

    // first generated CPM shall include the SensorInformationContainer
    mLastSensorInformationTimestamp = mLastCpmTimestamp - mAddSensorInformation;

    mObjectInclusionConfig = par("objectInclusionConfig");
    mObjectPerceptionQualityThreshold = par("objectPerceptionQualityThreshold");
    mMinPositionChangeThreshold = par("minPositionChangeThreshold").doubleValue() * vanetza::units::si::meter;
    mMinGroundSpeedChangeThreshold = par("minGroundSpeedChangeThreshold").doubleValue() * vanetza::units::si::meter_per_second;
    mMinGroundVelocityOrientationChangeThreshold =
        vanetza::units::Angle{par("minGroundVelocityOrientationChangeThreshold").doubleValue() * vanetza::units::degree};

    mMinPositionChangePriorityThreshold = par("minPositionChangePriorityThreshold").doubleValue() * vanetza::units::si::meter;
    mMaxPositionChangePriorityThreshold = par("maxPositionChangePriorityThreshold").doubleValue() * vanetza::units::si::meter;
    mMinGroundSpeedChangePriorityThreshold = par("minGroundSpeedChangePriorityThreshold").doubleValue() * vanetza::units::si::meter_per_second;
    mMaxGroundSpeedChangePriorityThreshold = par("maxGroundSpeedChangePriorityThreshold").doubleValue() * vanetza::units::si::meter_per_second;
    mMinGroundVelocityOrientationChangePriorityThreshold =
        vanetza::units::Angle{par("minGroundVelocityOrientationChangePriorityThreshold").doubleValue() * vanetza::units::degree};
    mMaxGroundVelocityOrientationChangePriorityThreshold =
        vanetza::units::Angle{par("maxGroundVelocityOrientationChangePriorityThreshold").doubleValue() * vanetza::units::degree};
    mMinLastInclusionTimePriorityThreshold = par("minLastInclusionTimePriorityThreshold");
    mMaxLastInclusionTimePriorityThreshold = par("maxLastInclusionTimePriorityThreshold");

    mUnusedObjectIdRetentionPeriod = par("unusedObjectIdRetentionPeriod");
    mHaveStationId = false;
    mLastStationId = 0;
    mLem2Cps.clear();
    mCps2Lem.assign(kCpmObjectIdSpace, kInvalidLemId);
    mCpmObjectIdLastUsed.assign(kCpmObjectIdSpace, kNeverUsedSimTime);
    mCpmObjectIdLastAgeMs.assign(kCpmObjectIdSpace, 0);

    mUnusedSensorIdRetentionPeriod = par("unusedSensorIdRetentionPeriod");
    mSensorId2Cps.clear();
    mCps2SensorId.assign(kCpmSensorIdSpace, kInvalidLemSensorId);
    mCpmSensorIdLastUsed.assign(kCpmSensorIdSpace, kNeverUsedSimTime);

    mDccRestriction = par("withDccRestriction");

    // look up primary channel for CP
    mPrimaryChannel = getFacilities().get_const<MultiChannelPolicy>().primaryChannel(vanetza::aid::CP);

    const auto stationType = mVehicleDataProvider->getStationType();
}

void CpService::indicate(const vanetza::btp::DataIndication& ind, std::unique_ptr<vanetza::UpPacket> packet)
{
    Enter_Method("indicate");

    Asn1PacketVisitor<Cpm> visitor;
    const Cpm* cpm = boost::apply_visitor(visitor, *packet);
    if (cpm && cpm->validate()) {
        CpObject obj = visitor.shared_wrapper;
        emit(scSignalCpmReceived, &obj);
    }
}

void CpService::trigger()
{
    Enter_Method("trigger");
    checkTriggeringConditions(simTime());
}

void CpService::checkTriggeringConditions(const SimTime& T_now)
{
    SimTime& T_GenCpm = mGenCpm;
    const SimTime& T_GenCpmMin = mGenCpmMin;
    const SimTime& T_GenCpmMax = mGenCpmMax;
    const SimTime T_GenCpmDcc = mDccRestriction ? genCpmDcc() : T_GenCpmMin;
    const SimTime T_elapsed = T_now - mLastCpmTimestamp;

    if (T_elapsed >= T_GenCpmDcc) {
        sendCpm(T_now);
    }
}

void CpService::sendCpm(const SimTime& T_now)
{
    captureVdpSnapshot();
    const auto lemSensorsSnapshot = mLocalEnvironmentModel->getSensors();
    const auto lemObjectsSnapshot = mLocalEnvironmentModel->allObjects();
    const auto objectVdpSnapshots = captureObjectVdpSnapshot(lemObjectsSnapshot);

    const auto referenceTime = countTaiMilliseconds(mTimer->getTimeFor(mVdpSnapshot.updated));

    const uint32_t stationId = mVdpSnapshot.stationId;
    if (!mHaveStationId) {
        mHaveStationId = true;
        mLastStationId = stationId;
    } else if (stationId != mLastStationId) {
        handlePseudonymChange();
        mLastStationId = stationId;
    }

    captureSensorSnapshot(T_now, lemSensorsSnapshot);
    capturePerceivedObjectSnapshot(T_now, referenceTime, lemObjectsSnapshot, objectVdpSnapshots);

    auto cpm = createCollectivePerceptionMessage(mVdpSnapshot, referenceTime);

    if (mVdpSnapshot.stationType == static_cast<int>(vanetza::geonet::StationType::RSU)) {
        addOriginatingRsuContainer(cpm);
    } else {
        addOriginatingVehicleContainer(cpm, mVdpSnapshot);
    }
    if (checkSensorInformationTrigger(T_now)) {
        addSensorInformationContainer(cpm, mSensorSnapshot);
        mLastSensorInformationTimestamp = T_now;
    }
    if (checkPerceptionRegionTrigger()) {
        addPerceptionRegionContainer(cpm);
    }
    if (checkPerceivedObjectTrigger(T_now)) {
        addPerceivedObjectContainer(cpm, mPerceivedObjectSnapshot, mSelectedCpmObjects, mLastCpmTimestamp);
    }

    std::string error;
    if (!cpm.validate(error)) {
        throw cRuntimeError("Invalid CPM: %s", error.c_str());
    }

    mLastCpmTimestamp = T_now;

    using namespace vanetza;
    btp::DataRequestB request;
    request.destination_port = btp::ports::CPM;
    request.gn.its_aid = aid::CP;
    request.gn.transport_type = geonet::TransportType::SHB;
    request.gn.maximum_lifetime = geonet::Lifetime{geonet::Lifetime::Base::One_Second, 1};
    request.gn.traffic_class.tc_id(static_cast<unsigned>(dcc::Profile::DP2));
    request.gn.communication_profile = geonet::CommunicationProfile::ITS_G5;

    CpObject obj(std::move(cpm));
    emit(scSignalCpmSent, &obj);

    using CpmByteBuffer = convertible::byte_buffer_impl<Cpm>;
    std::unique_ptr<geonet::DownPacket> payload{new geonet::DownPacket()};
    std::unique_ptr<convertible::byte_buffer> buffer{new CpmByteBuffer(obj.shared_ptr())};
    payload->layer(OsiLayer::Application) = std::move(buffer);
    this->request(request, std::move(payload));
}

void CpService::captureVdpSnapshot()
{
    mVdpSnapshot.updated = mVehicleDataProvider->updated();
    mVdpSnapshot.stationId = static_cast<uint32_t>(mVehicleDataProvider->getStationId());
    mVdpSnapshot.stationType = static_cast<int>(mVehicleDataProvider->getStationType());
    mVdpSnapshot.position = mVehicleDataProvider->position();
    mVdpSnapshot.referenceLatitude = round(mVehicleDataProvider->latitude(), vanetza::units::degree * boost::units::si::micro) * 10;
    mVdpSnapshot.referenceLongitude = round(mVehicleDataProvider->longitude(), vanetza::units::degree * boost::units::si::micro) * 10;
    mVdpSnapshot.orientationAngleDeciDeg = round(mVehicleDataProvider->heading(), vanetza::units::degree * boost::units::si::deci);
}

void CpService::captureSensorSnapshot(const omnetpp::SimTime& T_now, const std::vector<Sensor*>& sensors)
{
    // cache sensors + allocated CPS sensor IDs/types
    mSensorSnapshot.clear();
    mSensorSnapshot.reserve(sensors.size());

    auto mapSensorType = [](const Sensor& sensor) -> Vanetza_ITS2_SensorType_t {
        const std::string& category = sensor.getSensorCategory();
        if (category == "Radar") {
            return Vanetza_ITS2_SensorType_radar;
        }
        if (category == "CA") {
            return Vanetza_ITS2_SensorType_itsAggregation;
        }
        // FoV and see-through sensors don't have vanetza equivalents
        return Vanetza_ITS2_SensorType_undefined;
    };

    for (const Sensor* sensor : sensors) {
        if (!sensor) {
            EV_WARN << "null sensor pointer in LEM sensors";
            continue;
        }
        SensorSnapshot snap;
        snap.sensor = sensor;
        auto cpsId = allocateCpmSensorId(T_now, sensor->getId());
        if (!cpsId) {
            EV_WARN << "no available CPS sensor ID";
            continue;
        }
        snap.id = *cpsId;
        snap.type = mapSensorType(*sensor);
        snap.shadowingApplies = false;
        mSensorSnapshot.push_back(snap);
    }
}

CpService::ObjectVdpSnapshotMap CpService::captureObjectVdpSnapshot(const LocalEnvironmentModel::TrackedObjects& objects) const
{
    ObjectVdpSnapshotMap snapshots;
    snapshots.reserve(objects.size());

    for (const LocalEnvironmentModel::TrackedObject& obj : objects) {
        const LocalEnvironmentModel::Object& object = obj.first;
        const LocalEnvironmentModel::Tracking& tracking = obj.second;
        auto objPtr = object.lock();
        if (!objPtr) {
            continue;
        }

        ObjectVdpSnapshot snap;
        auto traciObj = std::dynamic_pointer_cast<artery::TraCIEnvironmentModelObject>(objPtr);
        if (traciObj) {
            const VehicleDataProvider& vdpObj = traciObj->getVehicleData();
            snap.hasVdpData = true;
            snap.speed = vdpObj.speed();
            snap.heading = vdpObj.heading();
            snap.stationType = vdpObj.getStationType();
        }

        snapshots.emplace(static_cast<uint32_t>(tracking.id()), std::move(snap));
    }

    return snapshots;
}

void CpService::capturePerceivedObjectSnapshot(
    const omnetpp::SimTime& T_now, uint64_t referenceTime, const LocalEnvironmentModel::TrackedObjects& objects, const ObjectVdpSnapshotMap& objectVdpSnapshots)
{
    mPerceivedObjectSnapshot.clear();
    mSelectedCpmObjects.clear();

    const Position& egoPos = mVdpSnapshot.position;

    // build a temporary map from Sensor* to CPS sensorId for filling the contributing sensor IDs in the object snapshot
    std::unordered_map<const Sensor*, uint8_t> sensorToCpsId;
    sensorToCpsId.reserve(mSensorSnapshot.size());
    for (const auto& snap : mSensorSnapshot) {
        if (snap.sensor) {
            sensorToCpsId.emplace(snap.sensor, snap.id);
        }
    }

    mPerceivedObjectSnapshot.reserve(objects.size());

    for (const LocalEnvironmentModel::TrackedObject& obj : objects) {
        const LocalEnvironmentModel::Object& object = obj.first;
        const LocalEnvironmentModel::Tracking& tracking = obj.second;
        auto objPtr = object.lock();
        if (!objPtr) {
            continue;
        }

        PerceivedObjectSnapshot snap;
        snap.lemId = static_cast<uint32_t>(tracking.id());

        // last detected time
        SimTime lastDetected = SimTime::ZERO;
        for (const auto& sensor : tracking.sensors()) {
            const LocalEnvironmentModel::TrackingTime& trackingTime = sensor.second;
            if (lastDetected < trackingTime.last()) {
                lastDetected = trackingTime.last();
            }
        }
        int64_t lastDetectedTaiMs = countTaiMilliseconds(mTimer->getTimeFor(lastDetected));
        int64_t deltaTime = referenceTime - lastDetectedTaiMs;
        deltaTime = std::clamp(deltaTime, int64_t(-2048), int64_t(2047));
        snap.measurementDeltaTimeMs = static_cast<long>(deltaTime);

        // first detected time
        SimTime firstDetected = SimTime::getMaxTime();
        for (const auto& sensor : tracking.sensors()) {
            const LocalEnvironmentModel::TrackingTime& trackingTime = sensor.second;
            if (firstDetected > trackingTime.first()) {
                firstDetected = trackingTime.first();
            }
        }
        if (firstDetected == SimTime::getMaxTime()) {
            firstDetected = lastDetected;
        }
        int64_t firstDetectedTaiMs = countTaiMilliseconds(mTimer->getTimeFor(firstDetected));
        int64_t age = referenceTime - firstDetectedTaiMs;
        age = std::clamp(age, int64_t(0), int64_t(2047));
        snap.objectAgeMs = age;

        // replace LEM identifier with standard compliant CPM object ID (0..65535, random, reuse holdoff)
        const auto cpsId = allocateCpmObjectId(T_now, snap.lemId, snap.objectAgeMs);
        if (!cpsId) {
            EV_WARN << "no available CPS object ID";
            continue;
        }
        snap.cpsId = *cpsId;

        const Position& objectPos = objPtr->getCentrePoint();
        snap.xCm = round(objectPos.x - egoPos.x, vanetza::units::si::meter * boost::units::si::centi);
        if (snap.xCm < Vanetza_ITS2_CartesianCoordinateLarge_negativeOutOfRange)
            snap.xCm = Vanetza_ITS2_CartesianCoordinateLarge_negativeOutOfRange;
        else if (snap.xCm > Vanetza_ITS2_CartesianCoordinateLarge_positiveOutOfRange)
            snap.xCm = Vanetza_ITS2_CartesianCoordinateLarge_positiveOutOfRange;

        snap.yCm = round(objectPos.y - egoPos.y, vanetza::units::si::meter * boost::units::si::centi);
        if (snap.yCm < Vanetza_ITS2_CartesianCoordinateLarge_negativeOutOfRange)
            snap.yCm = Vanetza_ITS2_CartesianCoordinateLarge_negativeOutOfRange;
        else if (snap.yCm > Vanetza_ITS2_CartesianCoordinateLarge_positiveOutOfRange)
            snap.yCm = Vanetza_ITS2_CartesianCoordinateLarge_positiveOutOfRange;

        snap.objectPerceptionQuality = Vanetza_ITS2_ObjectPerceptionQuality_fullConfidence;

        const auto vdpSnapshotIt = objectVdpSnapshots.find(snap.lemId);
        if (vdpSnapshotIt != objectVdpSnapshots.end() && vdpSnapshotIt->second.hasVdpData) {
            const ObjectVdpSnapshot& vdpSnapshot = vdpSnapshotIt->second;
            snap.hasVelocity = true;
            snap.speedMps = vdpSnapshot.speed.value();
            snap.headingDeg = vdpSnapshot.heading.value();
            snap.stationType = static_cast<int>(vdpSnapshot.stationType);
            snap.type = toPOType(vdpSnapshot.stationType);
        }

        // fill contributing CPS sensor IDs (skip sensors not present in mSensorSnapshot)
        snap.sensorIds.clear();
        snap.sensorIds.reserve(tracking.sensors().size());
        for (const auto& entry : tracking.sensors()) {
            const Sensor* sensor = entry.first;
            if (!sensor) {
                EV_WARN << "null sensor pointer in tracking sensors";
                continue;
            }
            auto it = sensorToCpsId.find(sensor);
            if (it == sensorToCpsId.end()) {
                EV_WARN << "sensor not found";
                continue;
            }
            snap.sensorIds.push_back(it->second);
        }

        snap.utility = 0.0;
        mPerceivedObjectSnapshot.push_back(std::move(snap));
        mSelectedCpmObjects.push_back(mPerceivedObjectSnapshot.size() - 1);
    }
}

bool CpService::checkSensorInformationTrigger(const SimTime& T_now)
{
    const SimTime& T_AddSensorInformation = mAddSensorInformation;
    return (T_now - mLastSensorInformationTimestamp >= T_AddSensorInformation);
}

bool CpService::checkPerceptionRegionTrigger()
{
    // the currently available sensors on artery do not provide dynamic perception regions to check differences on shape, confidence and shadowing
    // this container is never added to the CPM
    return false;
}

bool CpService::checkPerceivedObjectTrigger(const SimTime& T_now)
{
    if (mObjectInclusionConfig == 0) {
        // include all perceived objects by default (up to the CPM capacity)
        return !mPerceivedObjectSnapshot.empty();
    } else if (mObjectInclusionConfig != 1)
        EV_ERROR << "Invalid object inclusion configuration: " << mObjectInclusionConfig << endl;

    // mObjectInclusionConfig = 1

    if (mPerceivedObjectSnapshot.empty()) {
        return false;
    }

    // determine whether all type A objects should be included this cycle
    // global type A inclusion can be triggered by:
    // 1) at least one type A not included for mGenCpmMax / 2, or
    // 2) at least one newly detected type A since the last CPM
    bool includeAllTypeA = false;
    for (const auto idx : mSelectedCpmObjects) {
        const auto& po = mPerceivedObjectSnapshot[idx];
        if (po.type != PerceivedObjectType::TypeA) {
            continue;
        }

        if (po.objectAgeMs < (T_now - mLastCpmTimestamp).dbl() * 1000) {  // newly detected
            includeAllTypeA = true;
            break;  // early exit as soon as the global condition becomes true
        }

        auto it = mPerceivedObjectHistories.find(static_cast<int>(po.cpsId));
        if (it == mPerceivedObjectHistories.end()) {
            continue;
        }

        if (T_now - it->second.inclusion >= mGenCpmMax / 2) {
            includeAllTypeA = true;
            break;  // early exit as soon as the global condition becomes true
        }
    }

    std::vector<std::size_t> selected;
    selected.reserve(mSelectedCpmObjects.size());

    // loop over the perceived objects (snapshot)
    for (const auto idx : mSelectedCpmObjects) {
        auto& po = mPerceivedObjectSnapshot[idx];

        // reset cached metrics for this evaluation
        po.haveHistory = false;
        po.distanceDiffM = 0.0;
        po.speedDiffMps = 0.0;
        po.orientationDiffDeg = 0.0;
        po.sinceLastInclusionSeconds = 0.0;

        // hard gate: perception quality check applied before type-specific inclusion rules
        if (po.objectPerceptionQuality <= mObjectPerceptionQualityThreshold) {
            continue;
        }

        // get object type (A or B)
        if (po.type == PerceivedObjectType::Unknown) {
            EV_WARN << "perceived object type is unknown, skipping object";
            continue;
        }

        auto histIt = mPerceivedObjectHistories.find(static_cast<int>(po.cpsId));
        if (histIt != mPerceivedObjectHistories.end()) {
            const PerceivedObjectHistory& history = histIt->second;
            po.haveHistory = true;

            // compute metrics for inclusion rules and utility function
            po.sinceLastInclusionSeconds = (T_now - history.inclusion).dbl();
            Position objectPos(static_cast<double>(po.xCm), static_cast<double>(po.yCm));
            po.distanceDiffM = distance(objectPos, history.position).value() / 100.0;
            double objectSpeed = po.hasVelocity ? po.speedMps : 0.0;
            po.speedDiffMps = std::abs(objectSpeed - history.speed);
            double objectOrientation = po.hasVelocity ? po.headingDeg : 0.0;
            po.orientationDiffDeg = std::abs(objectOrientation - history.orientation.degree());
        }

        // type A (VRU pedestrians/cyclists) inclusion rules
        if (po.type == PerceivedObjectType::TypeA) {
            // rule 1a: include type A if newly detected after last CPM generation event (new object)
            // rule 1b: include all type A if global type A trigger is active
            if (includeAllTypeA) {  // triggered by at least one new type A or at least one type A aged beyond T_GenCpmMax/2
                selected.push_back(idx);
            }
            continue;
        }

        // type B (vehicles/motorcycles) inclusion rules
        if (po.haveHistory) {
            // type B object with history: evaluate rules 2a-2e using the precomputed metrics
            // rule 2a: include if newly detected after last CPM generation event (new object)
            if (po.objectAgeMs < (T_now - mLastCpmTimestamp).dbl() * 1000) {
                selected.push_back(idx);
                continue;
            }
            // rule 2b: include if position change exceeds threshold
            if (po.distanceDiffM > mMinPositionChangeThreshold.value()) {
                selected.push_back(idx);
                continue;
            }
            // rule 2c: include if speed change exceeds threshold
            if (po.speedDiffMps > mMinGroundSpeedChangeThreshold.value()) {
                selected.push_back(idx);
                continue;
            }
            // rule 2d: include if orientation change exceeds threshold
            if (po.orientationDiffDeg >= mMinGroundVelocityOrientationChangeThreshold.value()) {
                selected.push_back(idx);
                continue;
            }
            // rule 2e: include if time since last inclusion exceeds T_GenCpmMax
            if (po.sinceLastInclusionSeconds >= mGenCpmMax.dbl()) {
                selected.push_back(idx);
                continue;
            }
        } else {  // type B object with no history (new object)
            selected.push_back(idx);
            continue;
        }
    }

    mSelectedCpmObjects = std::move(selected);

    if (mSelectedCpmObjects.empty()) {
        return false;
    }

    // update history and compute utility per included object
    for (const auto idx : mSelectedCpmObjects) {
        auto& po = mPerceivedObjectSnapshot[idx];

        // objects with prior inclusion history already carry cached delta metrics, while objects without history (newly detected) have zeros in the delta
        // metrics
        po.utility = calculateUtilityFunction(po, po.distanceDiffM, po.speedDiffMps, po.orientationDiffDeg, po.sinceLastInclusionSeconds);

        mPerceivedObjectHistories[static_cast<int>(po.cpsId)] = PerceivedObjectHistory{
            T_now, Position(static_cast<double>(po.xCm), static_cast<double>(po.yCm)), (po.hasVelocity ? po.speedMps : 0.0),
            Angle((po.hasVelocity ? po.headingDeg : 0.0) * M_PI / 180.0)};
    }

    sortPerceivedObjects();

    return true;
}

double CpService::calculateUtilityFunction(
    const PerceivedObjectSnapshot& po, double distanceDiff, double speedDiff, double orientationDiff, double lastInclusionSeconds)
{
    static constexpr double kMaxObjectPerceptionQuality = static_cast<double>(Vanetza_ITS2_ObjectPerceptionQuality_fullConfidence);

    double ouf = 0.0;

    ouf += static_cast<double>(po.objectPerceptionQuality) / kMaxObjectPerceptionQuality;
    if (distanceDiff > mMinPositionChangePriorityThreshold.value()) {
        ouf += std::min(
            1.0, (distanceDiff - mMinPositionChangePriorityThreshold.value()) /
                     (mMaxPositionChangePriorityThreshold.value() - mMinPositionChangePriorityThreshold.value()));
    }
    if (speedDiff > mMinGroundSpeedChangePriorityThreshold.value()) {
        ouf += std::min(
            1.0, (speedDiff - mMinGroundSpeedChangePriorityThreshold.value()) /
                     (mMaxGroundSpeedChangePriorityThreshold.value() - mMinGroundSpeedChangePriorityThreshold.value()));
    }
    if (orientationDiff > mMinGroundVelocityOrientationChangePriorityThreshold.value()) {
        ouf += std::min(
            1.0, (orientationDiff - mMinGroundVelocityOrientationChangePriorityThreshold.value()) /
                     (mMaxGroundVelocityOrientationChangePriorityThreshold.value() - mMinGroundVelocityOrientationChangePriorityThreshold.value()));
    }
    if (lastInclusionSeconds > mMinLastInclusionTimePriorityThreshold.dbl()) {
        ouf += std::min(
            1.0, (lastInclusionSeconds - mMinLastInclusionTimePriorityThreshold.dbl()) /
                     (mMaxLastInclusionTimePriorityThreshold.dbl() - mMinLastInclusionTimePriorityThreshold.dbl()));
    }

    return ouf;
}

void CpService::sortPerceivedObjects()
{
    if (mSelectedCpmObjects.size() <= 1) {
        return;
    }
    std::sort(mSelectedCpmObjects.begin(), mSelectedCpmObjects.end(), [this](std::size_t a, std::size_t b) {
        const auto& leftObj = mPerceivedObjectSnapshot[a];
        const auto& rightObj = mPerceivedObjectSnapshot[b];

        // primary rule: new objects (no history) are prioritized
        if (leftObj.haveHistory != rightObj.haveHistory) {
            return !leftObj.haveHistory;
        }
        // secondary rule: higher utility function values are prioritized
        return leftObj.utility > rightObj.utility;
    });
}

SimTime CpService::genCpmDcc()
{
    // network interface may not be ready yet during initialization, so look it up at this later point
    auto netifc = mNetworkInterfaceTable->select(mPrimaryChannel);
    vanetza::dcc::TransmitRateThrottle* trc = netifc ? netifc->getDccEntity().getTransmitRateThrottle() : nullptr;
    if (!trc) {
        throw cRuntimeError("No DCC TRC found for CP's primary channel %i", mPrimaryChannel);
    }

    static const vanetza::dcc::TransmissionLite cp_tx(vanetza::dcc::Profile::DP2, 0);
    vanetza::Clock::duration interval = trc->interval(cp_tx);
    SimTime dcc{std::chrono::duration_cast<std::chrono::milliseconds>(interval).count(), SIMTIME_MS};
    return std::min(mGenCpmMax, std::max(mGenCpmMin, dcc));
}

void CpService::handlePseudonymChange()
{
    EV_INFO << "pseudonym change detected";

    mLem2Cps.clear();
    std::fill(mCps2Lem.begin(), mCps2Lem.end(), kInvalidLemId);

    // all CPM object IDs are replaced after pseudonym change. So any state keyed by old IDs must be cleared
    mPerceivedObjectHistories.clear();
    mPerceivedObjectSnapshot.clear();
    mSelectedCpmObjects.clear();

    // clear sensor ID mappings as well
    mSensorId2Cps.clear();
    std::fill(mCps2SensorId.begin(), mCps2SensorId.end(), kInvalidLemSensorId);
}

std::optional<uint16_t> CpService::allocateCpmObjectId(const omnetpp::SimTime& T_now, uint32_t lemId, int64_t objectAgeMs)
{
    const int64_t retentionSigned = mUnusedObjectIdRetentionPeriod.inUnit(SIMTIME_MS);
    const SimTime retention = retentionSigned > 0 ? SimTime(retentionSigned, SIMTIME_MS) : SimTime(0, SIMTIME_MS);

    // stage 1: reuse existing consistent mapping if still within the retention window
    auto it = mLem2Cps.find(lemId);
    if (it != mLem2Cps.end()) {
        const uint16_t cpsId = it->second;
        if (mCps2Lem[cpsId] == lemId) {
            const SimTime lastUsed = mCpmObjectIdLastUsed[cpsId];
            if (lastUsed == kNeverUsedSimTime || (T_now >= lastUsed && (T_now - lastUsed) < retention)) {
                mCpmObjectIdLastUsed[cpsId] = T_now;
                mCpmObjectIdLastAgeMs[cpsId] = objectAgeMs;
                return cpsId;
            }
        } else {
            EV_WARN << "object ID mapping inconsistency detected, reassigning ID";
        }
    }

    // an ID is available if it has never been used or has been idle for at least the retention period
    auto is_available = [&](uint16_t cpsId) {
        const SimTime lastUsed = mCpmObjectIdLastUsed[cpsId];
        if (lastUsed == kNeverUsedSimTime) {
            return true;
        }
        if (T_now < lastUsed) {
            EV_WARN << "timebase mismatch detected for object ID last used timestamp";
            return false;
        }
        return (T_now - lastUsed) >= retention;
    };
    // stage 2: bounded random probe
    bool found = false;
    uint16_t chosen = 0;
    for (int attempt = 0; attempt < 1024; ++attempt) {
        const uint16_t candidate = static_cast<uint16_t>(intuniform(0, static_cast<int>(kCpmObjectIdSpace - 1)));
        if (is_available(candidate)) {
            chosen = candidate;
            found = true;
            break;
        }
    }
    // stage 3: deterministic linear scan (guarantees finding an ID if one exists)
    if (!found) {
        for (std::size_t id = 0; id < kCpmObjectIdSpace; ++id) {
            if (is_available(static_cast<uint16_t>(id))) {
                chosen = static_cast<uint16_t>(id);
                found = true;
                break;
            }
        }
    }
    if (!found) {
        return std::nullopt;
    }

    // if the chosen ID was previously bound to another LEM object, remove stale state
    if (mCps2Lem[chosen] != kInvalidLemId) {
        mLem2Cps.erase(mCps2Lem[chosen]);
        mPerceivedObjectHistories.erase(static_cast<int>(chosen));
    }

    mLem2Cps[lemId] = chosen;
    mCps2Lem[chosen] = lemId;
    mCpmObjectIdLastUsed[chosen] = T_now;
    mCpmObjectIdLastAgeMs[chosen] = objectAgeMs;

    return chosen;
}

std::optional<uint8_t> CpService::allocateCpmSensorId(const omnetpp::SimTime& T_now, const int sensorId)
{
    // reuse existing mapping for this sensorId
    auto it = mSensorId2Cps.find(sensorId);
    if (it != mSensorId2Cps.end()) {
        const uint8_t cpsId = it->second;
        mCpmSensorIdLastUsed[cpsId] = T_now;
        return cpsId;
    }

    const int64_t retentionSigned = mUnusedSensorIdRetentionPeriod.inUnit(SIMTIME_MS);
    const SimTime retention = retentionSigned > 0 ? SimTime(retentionSigned, SIMTIME_MS) : SimTime(0, SIMTIME_MS);

    auto is_available = [&](uint8_t cpsId) {
        const SimTime lastUsed = mCpmSensorIdLastUsed[cpsId];
        if (lastUsed == kNeverUsedSimTime) {
            return true;
        }
        if (T_now < lastUsed) {
            EV_WARN << "timebase mismatch detected for sensor ID last used timestamp";
            return false;
        }
        // allow reuse after retention (even if currently mapped)
        return (T_now - lastUsed) >= retention;
    };

    bool found = false;
    uint8_t chosen = 0;
    for (int attempt = 0; attempt < 1024; ++attempt) {
        const int candidate = intuniform(0, static_cast<int>(kCpmSensorIdSpace - 1));
        chosen = static_cast<uint8_t>(candidate);
        if (is_available(chosen)) {
            found = true;
            break;
        }
    }
    if (!found) {
        for (std::size_t id = 0; id < kCpmSensorIdSpace; ++id) {
            chosen = static_cast<uint8_t>(id);
            if (is_available(chosen)) {
                found = true;
                break;
            }
        }
    }
    if (!found) {
        return std::nullopt;
    }

    // if reusing an ID which is still mapped, clear the old forward mapping first
    if (mCps2SensorId[chosen] != kInvalidLemSensorId) {
        mSensorId2Cps.erase(mCps2SensorId[chosen]);
    }

    mSensorId2Cps[sensorId] = chosen;
    mCps2SensorId[chosen] = sensorId;
    mCpmSensorIdLastUsed[chosen] = T_now;

    return chosen;
}

CpService::PerceivedObjectType CpService::toPOType(vanetza::geonet::StationType st)
{
    using vanetza::geonet::StationType;
    switch (st) {
        // type A: VRU pedestrian and bicyclist
        case StationType::Pedestrian:
        case StationType::Cyclist:
            return PerceivedObjectType::TypeA;

        // type B: everything else (including motorcyclist)
        default:
            return PerceivedObjectType::TypeB;
    }
}

Cpm createCollectivePerceptionMessage(const CpService::VdpSnapshot& vdp, uint64_t referenceTime)
{
    Cpm message;

    Vanetza_ITS2_ItsPduHeader_t& header = (*message).header;
    header.protocolVersion = 2;
    header.messageId = Vanetza_ITS2_MessageId_cpm;
    header.stationId = vdp.stationId;

    Vanetza_ITS2_CpmPayload_t& payload = (*message).payload;

    Vanetza_ITS2_CPM_PDU_Descriptions_ManagementContainer_t& mc = payload.managementContainer;

    int ret = asn_uint642INTEGER(&mc.referenceTime, referenceTime);
    assert(ret == 0);

    mc.referencePosition.latitude = vdp.referenceLatitude;
    mc.referencePosition.longitude = vdp.referenceLongitude;
    mc.referencePosition.positionConfidenceEllipse.semiMajorConfidence = Vanetza_ITS2_SemiAxisLength_unavailable;
    mc.referencePosition.positionConfidenceEllipse.semiMinorConfidence = Vanetza_ITS2_SemiAxisLength_unavailable;
    mc.referencePosition.positionConfidenceEllipse.semiMajorOrientation = Vanetza_ITS2_HeadingValue_unavailable;
    mc.referencePosition.altitude.altitudeValue = Vanetza_ITS2_AltitudeValue_unavailable;
    mc.referencePosition.altitude.altitudeConfidence = Vanetza_ITS2_AltitudeConfidence_unavailable;

    // TODO: add optional values

    return message;
}

void addOriginatingVehicleContainer(Cpm& message, const CpService::VdpSnapshot& vdp)
{
    Vanetza_ITS2_WrappedCpmContainer_t* wcc = vanetza::asn1::allocate<Vanetza_ITS2_WrappedCpmContainer_t>();
    wcc->containerId = Vanetza_ITS2_CpmContainerId_originatingVehicleContainer;
    wcc->containerData.present = Vanetza_ITS2_WrappedCpmContainer__containerData_PR_OriginatingVehicleContainer;

    Vanetza_ITS2_OriginatingVehicleContainer_t& ovc = wcc->containerData.choice.OriginatingVehicleContainer;
    ovc.orientationAngle.value = vdp.orientationAngleDeciDeg;
    ovc.orientationAngle.confidence = Vanetza_ITS2_Wgs84AngleConfidence_unavailable;

    // TODO: add optional values

    int ret = ASN_SEQUENCE_ADD(&(message->payload.cpmContainers.list), wcc);
    assert(ret == 0);

    std::string error;
    if (!message.validate(error)) {
        throw cRuntimeError("Invalid Originating Vehicle Container: %s", error.c_str());
    }
}

void addOriginatingRsuContainer(Cpm& message)
{
    Vanetza_ITS2_WrappedCpmContainer_t* wcc = vanetza::asn1::allocate<Vanetza_ITS2_WrappedCpmContainer_t>();
    wcc->containerId = Vanetza_ITS2_CpmContainerId_originatingRsuContainer;
    wcc->containerData.present = Vanetza_ITS2_WrappedCpmContainer__containerData_PR_OriginatingRsuContainer;

    Vanetza_ITS2_OriginatingRsuContainer_t& orc = wcc->containerData.choice.OriginatingRsuContainer;

    // TODO: add optional values

    int ret = ASN_SEQUENCE_ADD(&(message->payload.cpmContainers.list), wcc);
    assert(ret == 0);

    std::string error;
    if (!message.validate(error)) {
        throw cRuntimeError("Invalid Originating Rsu Container: %s", error.c_str());
    }
}

void addSensorInformationContainer(Cpm& message, const std::vector<CpService::SensorSnapshot>& sensorSnapshot)
{
    Vanetza_ITS2_WrappedCpmContainer_t* wcc = vanetza::asn1::allocate<Vanetza_ITS2_WrappedCpmContainer_t>();
    wcc->containerId = Vanetza_ITS2_CpmContainerId_sensorInformationContainer;
    wcc->containerData.present = Vanetza_ITS2_WrappedCpmContainer__containerData_PR_SensorInformationContainer;

    Vanetza_ITS2_SensorInformationContainer_t& sic = wcc->containerData.choice.SensorInformationContainer;

    for (const auto& snap : sensorSnapshot) {
        if (!snap.sensor) {
            continue;
        }
        Vanetza_ITS2_SensorInformation_t* si = vanetza::asn1::allocate<Vanetza_ITS2_SensorInformation_t>();
        si->sensorId = snap.id;
        si->sensorType = snap.type;
        si->shadowingApplies = snap.shadowingApplies;

        int ret = ASN_SEQUENCE_ADD(&sic.list, si);
        assert(ret == 0);
    }

    int ret = ASN_SEQUENCE_ADD(&(message->payload.cpmContainers.list), wcc);
    assert(ret == 0);

    std::string error;
    if (!message.validate(error)) {
        throw cRuntimeError("Invalid Sensor Information Container: %s", error.c_str());
    }
}

void addPerceptionRegionContainer(Cpm& message)
{
    // Vanetza_ITS2_WrappedCpmContainer_t* wcc = vanetza::asn1::allocate<Vanetza_ITS2_WrappedCpmContainer_t>();
    // wcc->containerId = Vanetza_ITS2_CpmContainerId_perceptionRegionContainer;
    // wcc->containerData.present = Vanetza_ITS2_WrappedCpmContainer__containerData_PR_PerceptionRegionContainer;

    // Vanetza_ITS2_PerceptionRegionContainer_t& prc = wcc->containerData.choice.PerceptionRegionContainer;

    // ...
}

void addPerceivedObjectContainer(
    Cpm& message, const std::vector<CpService::PerceivedObjectSnapshot>& perceivedObjectSnapshot, const std::vector<std::size_t>& selectedObjects,
    const omnetpp::SimTime& lastCpmTimestamp)
{
    Vanetza_ITS2_WrappedCpmContainer_t* wcc = vanetza::asn1::allocate<Vanetza_ITS2_WrappedCpmContainer_t>();
    wcc->containerId = Vanetza_ITS2_CpmContainerId_perceivedObjectContainer;
    wcc->containerData.present = Vanetza_ITS2_WrappedCpmContainer__containerData_PR_PerceivedObjectContainer;

    Vanetza_ITS2_PerceivedObjectContainer_t& poc = wcc->containerData.choice.PerceivedObjectContainer;

    const std::size_t total = selectedObjects.size();
    const std::size_t count = std::min(total, std::size_t(255));
    poc.numberOfPerceivedObjects = static_cast<long>(count);

    for (std::size_t i = 0; i < count; ++i) {
        const std::size_t idx = selectedObjects[i];
        if (idx >= perceivedObjectSnapshot.size()) {
            continue;
        }
        const auto& snap = perceivedObjectSnapshot[idx];

        auto* po = vanetza::asn1::allocate<Vanetza_ITS2_PerceivedObject_t>();
        po->objectId = vanetza::asn1::allocate<Vanetza_ITS2_Identifier2B_t>();
        *(po->objectId) = snap.cpsId;

        po->measurementDeltaTime = snap.measurementDeltaTimeMs;

        po->position.xCoordinate.value = snap.xCm;
        po->position.xCoordinate.confidence = Vanetza_ITS2_CoordinateConfidence_unavailable;
        if (po->position.xCoordinate.confidence > 4094 || po->position.xCoordinate.confidence < 1) {
            po->position.xCoordinate.confidence = Vanetza_ITS2_CoordinateConfidence_outOfRange;
        }

        po->position.yCoordinate.value = snap.yCm;
        po->position.yCoordinate.confidence = Vanetza_ITS2_CoordinateConfidence_unavailable;
        if (po->position.yCoordinate.confidence > 4094 || po->position.yCoordinate.confidence < 1) {
            po->position.yCoordinate.confidence = Vanetza_ITS2_CoordinateConfidence_outOfRange;
        }

        po->objectAge = vanetza::asn1::allocate<Vanetza_ITS2_DeltaTimeMilliSecondSigned_t>();
        *(po->objectAge) = static_cast<long>(snap.objectAgeMs);

        po->objectPerceptionQuality = vanetza::asn1::allocate<Vanetza_ITS2_ObjectPerceptionQuality_t>();
        *(po->objectPerceptionQuality) = snap.objectPerceptionQuality;

        po->velocity = nullptr;
        if (snap.hasVelocity) {
            po->velocity = vanetza::asn1::allocate<Vanetza_ITS2_Velocity3dWithConfidence_t>();
            po->velocity->present = Vanetza_ITS2_Velocity3dWithConfidence_PR_polarVelocity;
            auto& polar = po->velocity->choice.polarVelocity;
            polar.velocityMagnitude.speedValue = static_cast<long>(std::round(snap.speedMps * 100.0));
            polar.velocityMagnitude.speedConfidence = Vanetza_ITS2_SpeedConfidence_unavailable;
            polar.velocityDirection.value = static_cast<long>(std::round(snap.headingDeg * 10.0));
            polar.velocityDirection.confidence = Vanetza_ITS2_AngleConfidence_unavailable;
        }

        po->classification = nullptr;
        if (snap.stationType >= 0) {
            setPOClassification(*po, static_cast<vanetza::geonet::StationType>(snap.stationType));
        }

        po->sensorIdList = nullptr;
        if (!snap.sensorIds.empty()) {
            po->sensorIdList = vanetza::asn1::allocate<Vanetza_ITS2_SequenceOfIdentifier1B_t>();
            for (const uint8_t sidVal : snap.sensorIds) {
                auto* sid = vanetza::asn1::allocate<Vanetza_ITS2_Identifier1B_t>();
                *sid = sidVal;
                int ret = ASN_SEQUENCE_ADD(&po->sensorIdList->list, sid);
                assert(ret == 0);
            }
        }

        int ret = ASN_SEQUENCE_ADD(&poc.perceivedObjects.list, po);
        assert(ret == 0);
    }

    int ret = ASN_SEQUENCE_ADD(&(message->payload.cpmContainers.list), wcc);
    assert(ret == 0);

    std::string error;
    if (!message.validate(error)) {
        throw cRuntimeError("Invalid Perceived Object Container: %s", error.c_str());
    }
}


}  // namespace artery
