/*
 * Artery V2X Simulation Framework
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/cp/CpService.h"

#include "artery/application/Asn1PacketVisitor.h"
#include "artery/application/LocalDynamicMap.h"
#include "artery/application/MultiChannelPolicy.h"
#include "artery/application/NetworkInterfaceTable.h"
#include "artery/application/Timer.h"
#include "artery/application/VehicleDataProvider.h"
#include "artery/application/cp/CpObject.h"
#include "artery/application/cp/GenerationRule.h"
#include "artery/application/cp/RedundancyMitigationRule.h"
#include "artery/application/cp/Units.h"
#include "artery/envmod/EnvironmentModelObject.h"
#include "artery/envmod/LocalEnvironmentModel.h"
#include "artery/traci/Cast.h"
#include "artery/traci/VehicleController.h"
#include "traci/API.h"
#include "traci/GeoPosition.h"
#include "traci/Position.h"

#include <boost/units/cmath.hpp>
#include <boost/units/systems/si/io.hpp>
#include <omnetpp/cexception.h>
#include <omnetpp/clog.h>
#include <omnetpp/cmessage.h>
#include <omnetpp/csimulation.h>
#include <omnetpp/simutil.h>
#include <vanetza/access/access_category.hpp>
#include <vanetza/asn1/support/constr_TYPE.h>
#include <vanetza/btp/data_request.hpp>
#include <vanetza/btp/ports.hpp>
#include <vanetza/common/its_aid.hpp>
#include <vanetza/dcc/flow_control.hpp>
#include <vanetza/dcc/smoothing_channel_probe_processor.hpp>
#include <vanetza/dcc/transmission.hpp>
#include <vanetza/dcc/transmit_rate_control.hpp>
#include <vanetza/geonet/interface.hpp>
#include <vanetza/geonet/lifetime.hpp>
#include <vanetza/geonet/mib.hpp>
#include <vanetza/geonet/traffic_class.hpp>
#include <vanetza/net/osi_layer.hpp>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <tuple>
#include <type_traits>


namespace opp = omnetpp;


/*
 * FIXME: Define these constants locally until Vanetza contains them
 */
static constexpr long ItsPduHeader__messageID_cpm = 14;
static constexpr long ItsPduHeader__protocolVersion_cpm = 1;


/**
 * @brief Signals for inter-module communication
 */
namespace evt
{
static const auto cpmReceived = opp::cComponent::registerSignal("CpmReceived");
static const auto cpmSent = opp::cComponent::registerSignal("CpmSent");
}  // namespace evt

/**
 * @brief Signals for statistic collection
 */
namespace sc
{
static const auto cpmSent_positionX = opp::cComponent::registerSignal("CpmSent_positionX");
static const auto cpmSent_positionY = opp::cComponent::registerSignal("CpmSent_positionY");
static const auto cpmSent_sender = opp::cComponent::registerSignal("CpmSent_sender");
static const auto cpmSent_generatedPocCount = opp::cComponent::registerSignal("CpmSent_generatedPocCount");
static const auto cpmSent_mitigatedPocCount = opp::cComponent::registerSignal("CpmSent_mitigatedPocCount");
static const auto cpmSent_channelBusyRatio = opp::cComponent::registerSignal("CpmSent_channelBusyRatio");
static const auto cpmSent_segmentCount = opp::cComponent::registerSignal("CpmSent_segmentCount");
static const auto cpmSent_pocIncluded = opp::cComponent::registerSignal("CpmSent_pocIncluded");
static const auto cpmSent_poCountSegment = opp::cComponent::registerSignal("CpmSent_poCountSegment");
static const auto cpmSent_sicIncludedInSegment = opp::cComponent::registerSignal("CpmSent_sicIncludedInSegmentNr");
static const auto cpmSent_msgSize = opp::cComponent::registerSignal("CpmSent_msgSize");

static const auto cpm_10Hz_numSeenByRadar = opp::cComponent::registerSignal("CpService_10Hz_numSeenByRadar");
static const auto cpm_10Hz_numSeenByCam = opp::cComponent::registerSignal("CpService_10Hz_numSeenByCam");
static const auto cpm_10Hz_numSeenByCpm = opp::cComponent::registerSignal("CpService_10Hz_numSeenByCpm");
static const auto cpm_10Hz_numSeenByAll = opp::cComponent::registerSignal("CpService_10Hz_numSeenByAll");

static const auto cpmRecv_positionX = opp::cComponent::registerSignal("CpmRecv_positionX");
static const auto cpmRecv_positionY = opp::cComponent::registerSignal("CpmRecv_positionY");
static const auto cpmRecv_sender = opp::cComponent::registerSignal("CpmRecv_sender");
static const auto cpmRecv_delay = opp::cComponent::registerSignal("CpmRecv_delay");
static const auto cpmRecv_distToSender = opp::cComponent::registerSignal("CpmRecv_distToSender");
static const auto cpmRecv_objId = opp::cComponent::registerSignal("CpmRecv_objId");
static const auto cpmRecv_age = opp::cComponent::registerSignal("CpmRecv_age");
static const auto cpmRecv_distance = opp::cComponent::registerSignal("CpmRecv_distance");
static const auto cpmRecv_poCountSegment = opp::cComponent::registerSignal("CpmRecv_poCountSegment");
static const auto cpmRecv_timeSinceLastObjectUpdate = opp::cComponent::registerSignal("CpmRecv_poTimeSinceLastUpdate");
static const auto cpmRecv_distanceSinceLastObjectUpdate = opp::cComponent::registerSignal("CpmRecv_poDistanceSinceLastUpdate");
static const auto cpmRecv_poRedundancy = opp::cComponent::registerSignal("CpmRecv_poRedundancy");
static const auto cpmRecv_senderRedundancy = opp::cComponent::registerSignal("CpmRecv_senderRedundancy");
static const auto cpmRecv_senderHistoryOverflow = opp::cComponent::registerSignal("CpmRecv_senderHistoryOverflow");
static const auto cpmRecv_objectHistoryOverflow = opp::cComponent::registerSignal("CpmRecv_objectHistoryOverflow");
static const auto cpmRecv_senderDupeDrop = opp::cComponent::registerSignal("CpmRecv_senderDupeDrop");
static const auto cpmRecv_objectDupeDrop = opp::cComponent::registerSignal("CpmRecv_objectDupeDrop");
static const auto cpmRecv_senderDupeReplace = opp::cComponent::registerSignal("CpmRecv_senderDupeReplace");
static const auto cpmRecv_objectDupeReplace = opp::cComponent::registerSignal("CpmRecv_objectDupeReplace");
}  // namespace sc

/**
 * @brief Data Element limits defined by ETSI TR 103 562 V2.1.1 and ETSI TS 102 894-2 V1.3.1
 */
namespace de
{
static constexpr DistanceValue_t distanceValueMin = -132768;
static constexpr DistanceValue_t distanceValueMax = 132767;
static constexpr Latitude_t latitudeMin = -900000000;
static constexpr Latitude_t latitudeMax = 900000000;
static constexpr Longitude_t longitudeMin = -1800000000;
static constexpr Longitude_t longitudeMax = 1800000000;
static constexpr LongitudinalAccelerationValue_t longitudinalAccelerationValueMin = -160;
static constexpr LongitudinalAccelerationValue_t longitudinalAccelerationValueMax = 160;
static constexpr NumberOfPerceivedObjects_t numberOfPerceivedObjectsMin = 0;
static constexpr NumberOfPerceivedObjects_t numberOfPerceivedObjectsMax = 255;
static constexpr ObjectAge_t objectAgeMin = 0;
static constexpr ObjectAge_t objectAgeMax = 1500;
static constexpr SegmentCount_t segmentCountMin = 1;
static constexpr SegmentCount_t segmentCountMax = 127;
static constexpr SpeedValue_t speedValueMin = 0;
static constexpr SpeedValue_t speedValueMax = 16382;
static constexpr SpeedValueExtended_t speedValueExtendedMin = -16383;
static constexpr SpeedValueExtended_t speedValueExtendedMax = 16382;
static constexpr TimeOfMeasurement_t timeOfMeasurementMin = -1500;
static constexpr TimeOfMeasurement_t timeOfMeasurementMax = 1500;
static constexpr YawRateValue_t yawRateValueMin = -32766;
static constexpr YawRateValue_t yawRateValueMax = 32766;
}  // namespace de

/**
 * @brief Data Frame limits defined by ETSI TR 103 562 V2.1.1 and ETSI TS 102 894-2 V1.3.1
 */
namespace df
{
static constexpr size_t perceivedObjectContainerMin = 1;
static constexpr size_t perceivedObjectContainerMax = 128;
}  // namespace df


namespace artery
{

Define_Module(CpService);

CpService::CpService() :
    mGenerationRule(nullptr),
    mPrimaryChannel(channel::CCH),
    mNetworkInterfaceTable(nullptr),
    mVehicleDataProvider(nullptr),
    mVehicleController(nullptr),
    mTimer(nullptr),
    mLocalEnvironmentModel(nullptr),
    mLocalDynamicMap(nullptr),
    mGenCpmMin(100, opp::SIMTIME_MS),
    mGenCpmMax(1000, opp::SIMTIME_MS),
    mAddSensorInformation(1000, opp::SIMTIME_MS),
    mMtuCpm(1394),
    mDccProfile(vanetza::dcc::Profile::DP2),
    mWithDccRestriction(false),
    mDynamicRedundancyMitigation(false),
    mRedundancyLoad(0.25),
    mGenCpm(100, opp::SIMTIME_MS),
    mLastCpm(opp::SimTime::ZERO),
    mLastSensorInfoContainer(opp::SimTime::ZERO - mAddSensorInformation),
    mLocalTrackingCategory("Radar")
{
}

CpService::~CpService() = default;


int CpService::numInitStages() const
{
    return std::max(ItsG5Service::numInitStages(), 1);
}

void CpService::initialize(int stage)
{
    EV_TRACE << "initialize(...) stage: " << stage << "\n";

    ItsG5Service::initialize(stage);

    switch (stage) {
        case 0: {
            auto* parentModule = getParentModule();

            auto* generationModule = parentModule->getSubmodule("generation");
            mGenerationRule = dynamic_cast<cp::GenerationRule*>(generationModule);
            if (!mGenerationRule) {
                throw opp::cRuntimeError("%s is not of type GenerationRule", generationModule->getFullName());
            }

            int numRedundancyMitigationRules = parentModule->par("numRedundancyMitigationRules");
            mRedundancyMitigationRules.reserve(numRedundancyMitigationRules);
            for (int i = 0; i < numRedundancyMitigationRules; ++i) {
                auto* redundancyMitigationModule = parentModule->getSubmodule("redundancyMitigation", i);
                auto* redundancyMitigationRule = dynamic_cast<cp::RedundancyMitigationRule*>(redundancyMitigationModule);
                if (!redundancyMitigationRule) {
                    throw opp::cRuntimeError("%s is not of type RedundancyMitigationRule", redundancyMitigationModule->getFullName());
                }
                mRedundancyMitigationRules.emplace_back(redundancyMitigationRule);
            }

            mPrimaryChannel = getFacilities().get_const<MultiChannelPolicy>().primaryChannel(vanetza::aid::CP);
            mNetworkInterfaceTable = getFacilities().get_const_ptr<NetworkInterfaceTable>();
            mVehicleDataProvider = getFacilities().get_const_ptr<VehicleDataProvider>();
            mVehicleController = getFacilities().get_const_ptr<traci::VehicleController>();
            mTimer = getFacilities().get_const_ptr<Timer>();
            mLocalEnvironmentModel = getFacilities().get_const_ptr<LocalEnvironmentModel>();
            mLocalDynamicMap = getFacilities().get_const_ptr<LocalDynamicMap>();

            // The SimTime constructor that takes a cPar does not evaluate the unit and always uses seconds, construct manually to apply the correct unit
            mGenCpmMin = opp::SimTime(par("genCpmMin").intValue(), opp::SIMTIME_MS);
            mGenCpmMax = opp::SimTime(par("genCpmMax").intValue(), opp::SIMTIME_MS);
            mAddSensorInformation = opp::SimTime(par("addSensorInformation").intValue(), opp::SIMTIME_MS);

            mMtuCpm = par("mtuCpm");
            // According to the vanetza::geonet::map_tc_onto_profile() implementation at least the last 2 bits of the raw
            // TrafficClass value map directly to the DCC profile, every other value maps to DP3, this is sufficient
            // for the supported value range
            auto tcRaw = static_cast<uint8_t>(par("dccProfile"));
            mDccProfile = vanetza::geonet::map_tc_onto_profile(vanetza::geonet::TrafficClass(tcRaw));
            mWithDccRestriction = par("withDccRestriction");

            mDynamicRedundancyMitigation = par("dynamicRedundancyMitigation");
            mRedundancyLoad = par("redundancyLoad");

            EV_TRACE << "CpmMin: " << mGenCpmMin.ustr() << "\n";
            EV_TRACE << "CpmMax: " << mGenCpmMax.ustr() << "\n";
            EV_TRACE << "SensorInformation: " << mAddSensorInformation.ustr() << "\n";
            EV_TRACE << "MTU: " << mMtuCpm << "\n";
            EV_TRACE << "DCC Profile: " << static_cast<unsigned>(mDccProfile) << "\n";
            EV_TRACE << "DCC Restriction: " << mWithDccRestriction << "\n";
            EV_TRACE << "Dynamic Redundancy: " << mDynamicRedundancyMitigation << "\n";
            EV_TRACE << "Redundancy Load: " << mRedundancyLoad << "\n";

            mGenCpm = mGenCpmMin;
            // Vehicles can get inserted late in the simulation, this prevents the difference between the current timestamp
            // and the last timestamp to reach very high values
            mLastCpm = opp::simTime();
            // To ensure that the first CPM always includes the SensorInformationContainer
            mLastSensorInfoContainer = mLastCpm - mAddSensorInformation;

            EV_TRACE << "sizeof this: " << sizeof(*this) << "\n";
            EV_TRACE << "sizeof TrackedDynamics: " << sizeof(cp::TrackedDynamics) << "\n";
            EV_TRACE << "sizeof LocalDynamics: " << sizeof(cp::LocalDynamics) << "\n";
            EV_TRACE << "sizeof LocalObject: " << sizeof(cp::LocalObject) << "\n";
            EV_TRACE << "sizeof CpmDynamics: " << sizeof(cp::CpmDynamics) << "\n";
            EV_TRACE << "sizeof HistoryCpmDynamics: " << sizeof(cp::HistoryCpmDynamics) << "\n";
            EV_TRACE << "sizeof AggregateCpmDynamics: " << sizeof(cp::AggregateCpmDynamics) << "\n";
            EV_TRACE << "sizeof StationCpmDynamics: " << sizeof(cp::RemoteDynamics::StationCpmDynamics) << "\n";
            EV_TRACE << "sizeof StationCpmDynamics entry: " << sizeof(cp::RemoteDynamics::StationCpmDynamics::value_type) << "\n";
            EV_TRACE << "sizeof RemoteDynamics: " << sizeof(cp::RemoteDynamics) << "\n";
            EV_TRACE << "sizeof RemoteObject: " << sizeof(cp::RemoteObject) << "\n";
            break;
        }
    }
}

void CpService::finish()
{
    EV_TRACE << "finish(...)\n";

    ItsG5Service::finish();
}


void CpService::indicate(const vanetza::btp::DataIndication& ind, std::unique_ptr<vanetza::UpPacket> packet, const NetworkInterface& interface)
{
    Enter_Method("indicate");
    EV_TRACE << "indicate(...)\n";

    artery::Asn1PacketVisitor<vanetza::asn1::Cpm> cpmVisitor;
    const auto* cpm = boost::apply_visitor(cpmVisitor, *packet);
    if (!cpm) {
        // Received packet is not a CPM, this is a valid case

        return;
    }
    if (!cpm->validate()) {
        // Should not happen because this service is the only sender of CPM's and validates CPM's before transmission
        EV_WARN << "Invalid CPM received\n";

        return;
    }

    const auto now = opp::simTime();
    const auto& receiverPosition = mVehicleDataProvider->position();
    const auto& hdr = (*cpm)->header;
    const auto& payload = (*cpm)->cpm;

    if (opp::getEnvir()->isLoggingEnabled()) {
        EV_DETAIL << "CPM received\n";
        EV_DETAIL << "Recv receiver: " << mVehicleDataProvider->getStationId() << ", time: " << now.ustr() << ", pos: " << receiverPosition.x << " - "
                  << receiverPosition.y << "\n";
        if (opp::cLog::runtimeLogPredicate(this, opp::LOGLEVEL_DEBUG, nullptr)) {
            asn_fprint(stdout, &asn_DEF_CPM, cpm->operator->());
        }
    }

    // Like the CaService first send the CPM as event to update Envmod, then process the CPM
    CpObject obj(cpmVisitor.shared_wrapper);
    emit(evt::cpmReceived, &obj);

    // The bounding box processing of the evaluation script requires position statistics for each event number
    emit(sc::cpmRecv_positionX, cp::convertToUnit(receiverPosition.x, vanetza::units::si::meter));
    emit(sc::cpmRecv_positionY, cp::convertToUnit(receiverPosition.y, vanetza::units::si::meter));

    // Remote tracking needs to be clear of expired elements, otherwise it is possible that the buffer of dynamic data overflows
    synchronizeRemoteTracking(now);

    // Sender reference timestamp and CPM delay in SimTime for easier calculations
    const auto senderNow = mTimer->getTimeFor(mTimer->reconstructMilliseconds(payload.generationDeltaTime));
    const auto senderDelay = now - senderNow;

    // Sender dynamic properties
    const auto& mgm = payload.cpmParameters.managementContainer;
    const traci::TraCIGeoPosition senderGeoPosition{
        cp::convertToUnit(vanetza::units::GeoAngle(mgm.referencePosition.longitude * cp::units::microdecidegree), vanetza::units::degree),
        cp::convertToUnit(vanetza::units::GeoAngle(mgm.referencePosition.latitude * cp::units::microdecidegree), vanetza::units::degree)};
    const auto senderPosition = artery::position_cast(
        traci::Boundary(mVehicleController->getTraCI()->simulation.getNetBoundary()), mVehicleController->getTraCI()->convert2D(senderGeoPosition));
    const auto& vehicle = payload.cpmParameters.stationDataContainer->choice.originatingVehicleContainer;
    const auto senderSpeed =
        (vehicle.driveDirection == DriveDirection_forward ? vanetza::units::Velocity(vehicle.speed.speedValue * cp::units::centimeter_per_second)
                                                          : -vanetza::units::Velocity(vehicle.speed.speedValue * cp::units::centimeter_per_second));
    const auto senderHeading = vanetza::units::Angle(vehicle.heading.headingValue * cp::units::decidegree);

    if (opp::getEnvir()->isLoggingEnabled()) {
        EV_DETAIL << "Send sender: " << hdr.stationID << ", time: " << senderNow.ustr() << ", pos: " << senderPosition.x << " - " << senderPosition.y
                  << ", delay: " << senderDelay.ustr() << ", distance: " << artery::distance(receiverPosition, senderPosition) << ", speed: " << senderSpeed
                  << "\n";
    }

    // The sender is also a detected object by CPM, add to remote tracking
    auto& senderTracking = mRemoteTracking[hdr.stationID];
    const auto prevSenderDynamics = senderTracking.getLatestCpmDynamics();
    const auto currentSenderDynamics = senderTracking.addCpmDynamics(
        hdr.stationID, cp::CpmDynamics{now, senderPosition, cp::TrackedDynamics{senderNow, senderPosition, senderSpeed, senderHeading}});

    if (opp::getEnvir()->isLoggingEnabled()) {
        if (prevSenderDynamics) {
            EV_DETAIL << "Prev recvTime: " << prevSenderDynamics->received.ustr() << ", senderPos: " << prevSenderDynamics->senderPosition.x << " - "
                      << prevSenderDynamics->senderPosition.y << ", time: " << prevSenderDynamics->trackedDynamics.timestamp.ustr()
                      << ", pos: " << prevSenderDynamics->trackedDynamics.position.x << " - " << prevSenderDynamics->trackedDynamics.position.y
                      << ", speed: " << prevSenderDynamics->trackedDynamics.speed << "\n";
        }
        EV_DETAIL << "Curr recvTime: " << currentSenderDynamics.first.received.ustr() << ", senderPos: " << currentSenderDynamics.first.senderPosition.x
                  << " - " << currentSenderDynamics.first.senderPosition.y << ", time: " << currentSenderDynamics.first.trackedDynamics.timestamp.ustr()
                  << ", pos: " << currentSenderDynamics.first.trackedDynamics.position.x << " - " << currentSenderDynamics.first.trackedDynamics.position.y
                  << ", speed: " << currentSenderDynamics.first.trackedDynamics.speed << "\n";
        EV_DETAIL << "Redundancy stations: " << senderTracking.getCpmDynamics().size() << ", objects: " << senderTracking.getCpmDynamicsCount() << "\n";
    }

    if (currentSenderDynamics.second == cp::RemoteDynamics::CpmResult::DuplicateDrop) {
        EV_DETAIL << "Dropping duplicate CPM from: " << hdr.stationID << "\n";
        emit(sc::cpmRecv_senderDupeDrop, static_cast<unsigned long>(hdr.stationID));

        return;
    }
    if (currentSenderDynamics.second == cp::RemoteDynamics::CpmResult::DuplicateReplace) {
        EV_DETAIL << "Replacing duplicate CPM from: " << hdr.stationID << "\n";
        emit(sc::cpmRecv_senderDupeReplace, static_cast<unsigned long>(hdr.stationID));
    }

    // Emit age and distance as sender and object statistic for easier evaluation
    emit(sc::cpmRecv_sender, static_cast<unsigned long>(hdr.stationID));
    const auto senderAge = static_cast<long>(senderDelay.inUnit(opp::SIMTIME_MS));
    emit(sc::cpmRecv_delay, senderAge);
    emit(sc::cpmRecv_age, senderAge);
    const auto senderDistance = cp::convertToUnit(artery::distance(senderPosition, receiverPosition), vanetza::units::si::meter);
    emit(sc::cpmRecv_distToSender, senderDistance);
    emit(sc::cpmRecv_distance, senderDistance);
    if (currentSenderDynamics.second == cp::RemoteDynamics::CpmResult::HistoryReplace) {
        emit(sc::cpmRecv_senderHistoryOverflow, static_cast<unsigned long>(hdr.stationID));
    }
    emitRemoteObjectStatistics(senderTracking, prevSenderDynamics, currentSenderDynamics.first);

    // Process Perceived Object's
    emit(
        sc::cpmRecv_poCountSegment,
        static_cast<unsigned long>(payload.cpmParameters.perceivedObjectContainer ? payload.cpmParameters.perceivedObjectContainer->list.count : 0));
    if (payload.cpmParameters.perceivedObjectContainer) {
        for (int i = 0; i < payload.cpmParameters.perceivedObjectContainer->list.count; ++i) {
            const auto& po = *payload.cpmParameters.perceivedObjectContainer->list.array[i];
            if (po.objectID == mVehicleDataProvider->getStationId()) {
                // Skip ego vehicle from remote tracking
                continue;
            }

            // Calculate object information age in SimTime for easier calculations
            const auto objectInformationTimestamp = senderNow + opp::SimTime(po.timeOfMeasurement, opp::SIMTIME_MS);
            const auto objectInformationAge = now - objectInformationTimestamp;

            const auto objectPosition = artery::Position{
                vanetza::units::Length(po.xDistance.value * cp::units::centimeter) + senderPosition.x,
                vanetza::units::Length(po.yDistance.value * cp::units::centimeter) + senderPosition.y};
            // WARNING: This is from the IBR implementation, a proper solution would be to compose the speed from the x- and y- direction
            //          relative to the sender vehicle speed
            const auto objectSpeed = vanetza::units::Velocity(po.xSpeed.value * cp::units::centimeter_per_second);
            // INFO: Currently hardcoded because the required data to calculate this is not added
            const auto objectHeading = vanetza::units::Angle(0 * cp::units::decidegree);

            if (opp::getEnvir()->isLoggingEnabled()) {
                EV_DETAIL << "Object id: " << po.objectID << ", time: " << objectInformationTimestamp.ustr() << ", pos: " << objectPosition.x << " - "
                          << objectPosition.y << ", infoAge: " << objectInformationAge.ustr()
                          << ", distance: " << artery::distance(objectPosition, receiverPosition) << ", speed: " << objectSpeed << "\n";
            }

            auto& objectTracking = mRemoteTracking[po.objectID];
            const auto prevObjectDynamics = objectTracking.getLatestCpmDynamics();
            const auto currentObjectDynamics = objectTracking.addCpmDynamics(
                hdr.stationID,
                cp::CpmDynamics{now, senderPosition, cp::TrackedDynamics{objectInformationTimestamp, objectPosition, objectSpeed, objectHeading}});

            if (opp::getEnvir()->isLoggingEnabled()) {
                if (prevObjectDynamics) {
                    EV_DETAIL << "Prev recvTime: " << prevObjectDynamics->received.ustr() << ", senderPos: " << prevObjectDynamics->senderPosition.x << " - "
                              << prevObjectDynamics->senderPosition.y << ", time: " << prevObjectDynamics->trackedDynamics.timestamp.ustr()
                              << ", pos: " << prevObjectDynamics->trackedDynamics.position.x << " - " << prevObjectDynamics->trackedDynamics.position.y
                              << ", speed: " << prevObjectDynamics->trackedDynamics.speed << "\n";
                }
                EV_DETAIL << "Curr recvTime: " << currentObjectDynamics.first.received.ustr() << ", senderPos: " << currentObjectDynamics.first.senderPosition.x
                          << " - " << currentObjectDynamics.first.senderPosition.y << ", time: " << currentObjectDynamics.first.trackedDynamics.timestamp.ustr()
                          << ", pos: " << currentObjectDynamics.first.trackedDynamics.position.x << " - "
                          << currentObjectDynamics.first.trackedDynamics.position.y << ", speed: " << currentObjectDynamics.first.trackedDynamics.speed << "\n";
                EV_DETAIL << "Redundancy stations: " << objectTracking.getCpmDynamics().size() << ", objects: " << objectTracking.getCpmDynamicsCount() << "\n";
            }

            if (currentObjectDynamics.second == cp::RemoteDynamics::CpmResult::DuplicateDrop) {
                EV_DETAIL << "Dropping duplicate object: " << po.objectID << " of CPM from: " << hdr.stationID << "\n";
                emit(sc::cpmRecv_objectDupeDrop, static_cast<unsigned long>(po.objectID));

                continue;
            }
            if (currentObjectDynamics.second == cp::RemoteDynamics::CpmResult::DuplicateReplace) {
                EV_DETAIL << "Replacing duplicate object: " << po.objectID << " of CPM from: " << hdr.stationID << "\n";
                emit(sc::cpmRecv_objectDupeReplace, static_cast<unsigned long>(po.objectID));
            }

            emit(sc::cpmRecv_objId, static_cast<unsigned long>(po.objectID));
            emit(sc::cpmRecv_age, static_cast<long>(objectInformationAge.inUnit(opp::SIMTIME_MS)));
            emit(sc::cpmRecv_distance, cp::convertToUnit(artery::distance(objectPosition, receiverPosition), vanetza::units::si::meter));
            if (currentObjectDynamics.second == cp::RemoteDynamics::CpmResult::HistoryReplace) {
                emit(sc::cpmRecv_objectHistoryOverflow, static_cast<unsigned long>(po.objectID));
            }
            emitRemoteObjectStatistics(objectTracking, prevObjectDynamics, currentObjectDynamics.first);
        }
    }
}

void CpService::trigger()
{
    Enter_Method("trigger");
    EV_TRACE << "trigger(...)\n";

    const auto now = opp::simTime();
    const auto& senderPosition = mVehicleDataProvider->position();

    // The bounding box processing of the evaluation script requires position statistics for each event number,
    // emit them here so that following code doesn't need to take care of this and to prevent to emit them multiple times
    emit(sc::cpmSent_positionX, cp::convertToUnit(senderPosition.x, vanetza::units::si::meter));
    emit(sc::cpmSent_positionY, cp::convertToUnit(senderPosition.y, vanetza::units::si::meter));

    // Synchronize object data already here to be able to generate statistics
    synchronizeLocalTracking(now);
    synchronizeRemoteTracking(now);

    // Count objects per class for statistics
    std::vector<StationID_t> uniqueObjects;
    uniqueObjects.reserve(mLocalTracking.size() + mRemoteTracking.size());
    size_t camObjects = 0;
    size_t cpmObjects = 0;
    for (const auto& localObject : mLocalTracking) { uniqueObjects.emplace_back(localObject.first.lock()->getVehicleData().getStationId()); }
    for (const auto& remoteObject : mRemoteTracking) {
        uniqueObjects.emplace_back(remoteObject.first);
        if (remoteObject.second.getCam()) {
            ++camObjects;
        }
        if (remoteObject.second.hasCpmDynamics()) {
            ++cpmObjects;
        }
    }
    std::sort(uniqueObjects.begin(), uniqueObjects.end());
    uniqueObjects.erase(std::unique(uniqueObjects.begin(), uniqueObjects.end()), uniqueObjects.end());
    emit(sc::cpm_10Hz_numSeenByRadar, static_cast<unsigned long>(mLocalTracking.size()));
    emit(sc::cpm_10Hz_numSeenByCam, static_cast<unsigned long>(camObjects));
    emit(sc::cpm_10Hz_numSeenByCpm, static_cast<unsigned long>(cpmObjects));
    emit(sc::cpm_10Hz_numSeenByAll, static_cast<unsigned long>(uniqueObjects.size()));

    if (checkTriggeringConditions(now)) {
        EV_DEBUG << "CPM creation triggered\n";

        // The generation rule requires current and especially expired-pointer-free local tracking data
        // TODO: For now this is already done above
        // synchronizeLocalTracking(now);

        auto pocCandidates = mGenerationRule->getPocCandidates(mLocalTracking, now, mGenCpm);
        const auto generatedPocs = pocCandidates.first.size();
        if (!mDynamicRedundancyMitigation || getCurrentCbr() > mRedundancyLoad) {
            EV_DEBUG << "Applying Redundancy Mitigation\n";

            // The redundancy mitigation rules require current remote tracking data
            // TODO: For now this is already done above
            // synchronizeRemoteTracking(now);

            for (auto& redundancyMitigationRule : mRedundancyMitigationRules) {
                redundancyMitigationRule->filterPocCandidates(pocCandidates.first, mRemoteTracking, *mVehicleDataProvider, now);
            }
        }
        const auto mitigatedPocs = pocCandidates.first.size();
        auto sic =
            (checkSensorInfoConditions(now) ? createSensorInformationContainer()
                                            : cp::make_empty_asn1<SensorInformationContainer>(asn_DEF_SensorInformationContainer));

        emit(sc::cpmSent_generatedPocCount, static_cast<unsigned long>(generatedPocs));
        emit(sc::cpmSent_mitigatedPocCount, static_cast<unsigned long>(mitigatedPocs));

        if (!pocCandidates.first.empty() || pocCandidates.second || sic) {
            generateCollectivePerceptionMessage(now, pocCandidates.first, std::move(sic));
        }
    }
}


bool CpService::checkTriggeringConditions(const omnetpp::SimTime& now)
{
    const auto elapsed = now - mLastCpm;
    const auto genCpmDcc = (mWithDccRestriction ? generateCpmDcc() : mGenCpmMin);

    if (elapsed >= genCpmDcc) {
        // Generation Rules or Redundancy Mitigation Rules might need to know
        // the projected generation time of the next CPM
        mGenCpm = genCpmDcc;

        return true;
    }

    return false;
}

bool CpService::checkSensorInfoConditions(const omnetpp::SimTime& now)
{
    return (now - mLastSensorInfoContainer >= mAddSensorInformation);
}

void CpService::synchronizeLocalTracking(const omnetpp::SimTime& now)
{
    EV_DEBUG << "Synchronizing local tracking\n";

    // To synchronize the LEM and LocalTracking leverage the fact that both maps use the same key element with the same ordering,
    // this allows to make a parallel, single run over the maps. The filtering does not change the order of the elements.
    // This assert tries to verify this fact, however it is too strict, both comparators do not need to be of the same type
    // but offer the same properties, however this is not so easy to test. Therefor test for the common case that both maps simply use the same comparator.
    static_assert(std::is_same<LocalEnvironmentModel::TrackedObjects::key_compare, cp::LocalTracking::key_compare>::value, "Incompatible comparators");
    // Can't compare the iterators of the two maps directly so use the comparator to compare their keys only
    auto cmp = LocalEnvironmentModel::TrackedObjects::key_compare();
    // The LEM contains not only object detected by physical sensors but also by virtual sensors like CAM or CPM.
    // We only need the objects from the physical sensors, right now only filtering by one category is available.
    auto localLem = filterBySensorCategory(mLocalEnvironmentModel->allObjects(), mLocalTrackingCategory);

    // The LEM enforces which objects are valid, update the local tracking accordingly
    auto lemObject = localLem.begin();
    auto localObject = mLocalTracking.begin();
    for (; lemObject != localLem.end() || localObject != mLocalTracking.end();) {
        if (lemObject == localLem.end()) {
            // End of LEM reached, all remaining local objects are expired, remove them
            // EV_TRACE << "End of LEM\n";
            if (localObject != mLocalTracking.end()) {
                localObject = mLocalTracking.erase(localObject, mLocalTracking.end());
            }
            continue;
        }
        if (localObject != mLocalTracking.end()) {
            // LEM and Local object present
            if (cmp(lemObject->first, localObject->first)) {
                // LEM object not present in local, insert
                // EV_TRACE << "New LEM object: " << lemObject->first.lock().get() << "\n";
                // Fallthrough to insert case
            } else if (cmp(localObject->first, lemObject->first)) {
                // Local object not present in LEM, remove
                // EV_TRACE << "Expired local object: " << localObject->first.lock().get() << "\n";
                localObject = mLocalTracking.erase(localObject);
                continue;
            } else {
                // Both objects are the same, update
                // EV_TRACE << "Equal objects: LEM " << lemObject->first.lock().get() << " - Local " << localObject->first.lock().get() << "\n";
                if (lemObject->first.expired()) {
                    // Object expired, remove
                    // Can't modify LEM, just skip
                    ++lemObject;
                    localObject = mLocalTracking.erase(localObject);
                } else {
                    // Object valid, update and advance
                    ++lemObject;
                    ++localObject;
                }
                continue;
            }
        }  // else End of local reached, add LEM object

        // Add only valid objects
        if (!lemObject->first.expired()) {
            // EV_TRACE << "Adding local object: " << lemObject->first.lock().get() << " - station: " <<
            // lemObject->first.lock()->getVehicleData().getStationId()
            //          << "\n";
            localObject = mLocalTracking.emplace_hint(localObject, std::piecewise_construct, std::forward_as_tuple(lemObject->first), std::forward_as_tuple());
            // Advance to prevent processing of the inserted object in next iteration
            ++localObject;
        }
        ++lemObject;
    }
}

void CpService::synchronizeRemoteTracking(const omnetpp::SimTime& now)
{
    EV_DEBUG << "Synchronizing remote tracking\n";

    // To synchronize RemoteTracking CAM objects leverage the fact that both maps use the same key element with the same ordering,
    // this allows to make a parallel, single run over the maps. The filtering does not change the order of the elements.
    // This assert tries to verify this fact, however it is too strict, both comparators do not need to be of the same type
    // but offer the same properties, however this is not so easy to test. Therefor test for the common case that both maps simply use the same comparator.
    static_assert(std::is_same<cp::RemoteTracking::key_compare, LocalDynamicMap::AwarenessEntries::key_compare>::value, "Incompatible comparators");
    // Can't compare the iterators of the two maps directly so use the comparator to compare their keys only
    auto cmp = cp::RemoteTracking::key_compare();

    const auto& cams = mLocalDynamicMap->allEntries();
    auto remoteObject = mRemoteTracking.begin();
    auto camObject = cams.begin();
    for (; remoteObject != mRemoteTracking.end();) {
        if (camObject != cams.end()) {
            if (cmp(camObject->first, remoteObject->first)) {
                // No remote object for CAM object, insert
                remoteObject = mRemoteTracking.emplace_hint(remoteObject, camObject->first, &(camObject->second.cam()));
                ++remoteObject;
                ++camObject;

                continue;
            } else if (cmp(remoteObject->first, camObject->first)) {
                // No CAM object for remote object, remove CAM
                if (remoteObject->second.getCam()) {
                    remoteObject->second.setCam(nullptr);
                }
                // Fallthrough to check remote object
            } else {
                // Both objects match, update
                if (remoteObject->second.getCam() != &(camObject->second.cam())) {
                    remoteObject->second.setCam(&(camObject->second.cam()));
                }
                ++camObject;
                // Fallthrough to check remote object
            }
        } else {
            // No CAM object for remote object, remove CAM
            if (remoteObject->second.getCam()) {
                remoteObject->second.setCam(nullptr);
            }
            // Fallthrough to check remote object
        }

        remoteObject->second.clearExpired(now);
        if (remoteObject->second.isExpired()) {
            remoteObject = mRemoteTracking.erase(remoteObject);
        } else {
            ++remoteObject;
        }
    }
}

void CpService::generateCollectivePerceptionMessage(const omnetpp::SimTime& now, cp::PocCandidates& pocCandidates, cp::SensorInformationContainer_ptr sic)
{
    EV_DEBUG << "Creating CPM, candidates: " << pocCandidates.size() << ", sic: " << !!sic << "\n";

    auto numberOfPerceivedObjects = mLocalTracking.size();
    if (numberOfPerceivedObjects > de::numberOfPerceivedObjectsMax) {
        EV_ERROR << "Number of Perceived Objects exceeds maximum, capping by " << (numberOfPerceivedObjects - de::numberOfPerceivedObjectsMax) << "\n";
        numberOfPerceivedObjects = de::numberOfPerceivedObjectsMax;
    }
    if (pocCandidates.size() > de::numberOfPerceivedObjectsMax) {
        EV_ERROR << "Perceived Objects exceeding maximum size, removing " << (pocCandidates.size() - de::numberOfPerceivedObjectsMax) << " objects\n";
        pocCandidates.erase(pocCandidates.begin() + de::numberOfPerceivedObjectsMax, pocCandidates.end());
    }
    auto pos = createPerceivedObjects(pocCandidates);

    mLastCpm = now;
    if (sic) {
        mLastSensorInfoContainer = now;
    }

    // Optimize for the common case, try a single CPM first
    if (pos.size() <= df::perceivedObjectContainerMax) {
        auto cpm = createCollectivePerceptionMessage(numberOfPerceivedObjects, false);

        if (sic) {
            cp::asn1_field_set(cpm->cpm.cpmParameters.sensorInformationContainer, sic);
        }
        if (!pos.empty()) {
            cpm->cpm.cpmParameters.perceivedObjectContainer = vanetza::asn1::allocate<PerceivedObjectContainer>();
            while (!cp::asn1_sequence_push(cpm->cpm.cpmParameters.perceivedObjectContainer, pos)) {};
        }

        const auto size = cpm.size();
        if (size <= mMtuCpm) {
            if (opp::getEnvir()->isLoggingEnabled()) {
                EV_DETAIL << "CPM transmitting, single\n";
                EV_DETAIL << "Trns sender: " << mVehicleDataProvider->getStationId() << ", time: " << now.ustr()
                          << ", pos: " << mVehicleDataProvider->position().x << " - " << mVehicleDataProvider->position().y
                          << ", delta : " << mVehicleDataProvider->updated().ustr() << "\n";
                if (opp::cLog::runtimeLogPredicate(this, opp::LOGLEVEL_DEBUG, nullptr)) {
                    asn_fprint(stdout, &asn_DEF_CPM, cpm.operator->());
                }
            }
            emit(sc::cpmSent_channelBusyRatio, getCurrentCbr());
            emit(sc::cpmSent_segmentCount, static_cast<unsigned long>(1));
            emit(sc::cpmSent_pocIncluded, !pocCandidates.empty());

            std::string error;
            if (!cpm.validate(error)) {
                throw opp::cRuntimeError("Invalid CPM constructed: %s", error.c_str());
            }

            emit(sc::cpmSent_msgSize, size);
            emit(
                sc::cpmSent_poCountSegment,
                static_cast<unsigned long>(cpm->cpm.cpmParameters.perceivedObjectContainer ? cpm->cpm.cpmParameters.perceivedObjectContainer->list.count : 0));
            emit(sc::cpmSent_sicIncludedInSegment, static_cast<unsigned long>(cpm->cpm.cpmParameters.sensorInformationContainer ? 1 : 0));
            emit(sc::cpmSent_sender, static_cast<unsigned long>(cpm->header.stationID));

            transmitCpm(std::move(cpm));
            updateLocalTracking(now, pocCandidates);

            return;
        }

        // Extract the sic and pos to use them for the segmented CPM case, otherwise they would get deleted when cpm goes out of scope
        if (cpm->cpm.cpmParameters.sensorInformationContainer) {
            cp::asn1_field_get(cpm->cpm.cpmParameters.sensorInformationContainer, sic);
        }
        if (cpm->cpm.cpmParameters.perceivedObjectContainer) {
            while (!cp::asn1_sequence_pop(cpm->cpm.cpmParameters.perceivedObjectContainer, pos)) {};
        }
    }

    // Segmented CPM case
    // Sort high priority elements to the end because the elements get processed from the end
    pos.sort([](const PerceivedObject& lhs, const PerceivedObject& rhs) {
        // According to ETSI TR 103 562 V2.1.1 ObjectConfidence_unknown is a valid value, since its value is zero
        // objects with this confidence get always priority value zero, their speed doesn't matter
        // WARNING: The IBR implementation stores the speed directly in the xSpeed and ySpeed variables
        const auto lPriority = (lhs.objectConfidence < ObjectConfidence_unavailable ? lhs.objectConfidence * lhs.xSpeed.value : lhs.xSpeed.value);
        const auto rPriority = (rhs.objectConfidence < ObjectConfidence_unavailable ? rhs.objectConfidence * rhs.xSpeed.value : rhs.xSpeed.value);
        return (lPriority < rPriority);
    });

    // If the MTU is too small to fit even a single Perceived Object this results in an infinite loop
    std::vector<vanetza::asn1::Cpm> cpms;
    cpms.reserve(8);
    while (!pos.empty()) {
        auto cpm = createCollectivePerceptionMessage(numberOfPerceivedObjects, true);

        cpm->cpm.cpmParameters.perceivedObjectContainer = vanetza::asn1::allocate<PerceivedObjectContainer>();
        for (int i = 0; i < df::perceivedObjectContainerMax && !pos.empty(); ++i) {
            cp::asn1_sequence_push(cpm->cpm.cpmParameters.perceivedObjectContainer, pos);
            if (cpm.size() > mMtuCpm) {
                cp::asn1_sequence_pop(cpm->cpm.cpmParameters.perceivedObjectContainer, pos);

                break;
            }
        }

        if (sic) {
            cp::asn1_field_set(cpm->cpm.cpmParameters.sensorInformationContainer, sic);
            if (cpm.size() > mMtuCpm) {
                cp::asn1_field_get(cpm->cpm.cpmParameters.sensorInformationContainer, sic);
            }
        }

        cpms.emplace_back(std::move(cpm));
    }
    if (sic) {
        auto cpm = createCollectivePerceptionMessage(numberOfPerceivedObjects, true);

        cp::asn1_field_set(cpm->cpm.cpmParameters.sensorInformationContainer, sic);

        cpms.emplace_back(std::move(cpm));
    }

    if (cpms.size() > de::segmentCountMax) {
        EV_ERROR << "Number of CPM segments exceeds maximum, capping by " << (cpms.size() - de::segmentCountMax) << "\n";
        cpms.erase(cpms.begin() + de::segmentCountMax, cpms.end());
    }
    for (size_t i = 0; i < cpms.size(); ++i) {
        auto& cpm = cpms[i];
        cpm->cpm.cpmParameters.managementContainer.perceivedObjectContainerSegmentInfo->totalMsgSegments = cpms.size();
        cpm->cpm.cpmParameters.managementContainer.perceivedObjectContainerSegmentInfo->thisSegmentNum = i + 1;
    }

    if (opp::getEnvir()->isLoggingEnabled()) {
        EV_DETAIL << "CPM transmitting, segments: " << cpms.size() << "\n";
        EV_DETAIL << "Trns sender: " << mVehicleDataProvider->getStationId() << ", time: " << now.ustr() << ", pos: " << mVehicleDataProvider->position().x
                  << " - " << mVehicleDataProvider->position().y << ", delta : " << mVehicleDataProvider->updated().ustr() << "\n";
    }
    emit(sc::cpmSent_channelBusyRatio, getCurrentCbr());
    emit(sc::cpmSent_segmentCount, static_cast<unsigned long>(cpms.size()));
    emit(sc::cpmSent_pocIncluded, !pocCandidates.empty());

    std::string error;
    for (auto& cpm : cpms) {
        const auto size = cpm.size();

        if (opp::cLog::runtimeLogPredicate(this, opp::LOGLEVEL_DEBUG, nullptr)) {
            asn_fprint(stdout, &asn_DEF_CPM, cpm.operator->());
        }
        if (!cpm.validate(error)) {
            throw opp::cRuntimeError("Invalid CPM constructed: %s", error.c_str());
        }

        emit(sc::cpmSent_msgSize, size);
        emit(
            sc::cpmSent_poCountSegment,
            static_cast<unsigned long>(cpm->cpm.cpmParameters.perceivedObjectContainer ? cpm->cpm.cpmParameters.perceivedObjectContainer->list.count : 0));
        if (cpm->cpm.cpmParameters.sensorInformationContainer) {
            emit(
                sc::cpmSent_sicIncludedInSegment,
                static_cast<unsigned long>(cpm->cpm.cpmParameters.managementContainer.perceivedObjectContainerSegmentInfo->thisSegmentNum));
        }
        emit(sc::cpmSent_sender, static_cast<unsigned long>(cpm->header.stationID));

        transmitCpm(std::move(cpm));
    }
    updateLocalTracking(now, pocCandidates);
}


void CpService::updateLocalTracking(const omnetpp::SimTime& now, cp::PocCandidates& pocCandidates)
{
    EV_DEBUG << "Updating local tracking\n";

    for (auto& cand : pocCandidates) {
        const auto object = cand->first.lock();
        const auto& vehicle = object->getVehicleData();

        cand->second.setPrevDynamics(cp::TrackedDynamics{now, vehicle.position(), vehicle.speed(), vehicle.heading()});
    }
}


vanetza::asn1::Cpm CpService::createCollectivePerceptionMessage(NumberOfPerceivedObjects_t numberOfPerceivedObjects, bool segmented)
{
    vanetza::asn1::Cpm cpm;

    // ITS PDU Header
    auto& hdr = cpm->header;
    hdr.protocolVersion = ItsPduHeader__protocolVersion_cpm;
    hdr.messageID = ItsPduHeader__messageID_cpm;
    hdr.stationID = mVehicleDataProvider->getStationId();

    // Basic fields
    // Downcast to 16 bit as cheap modulo operation
    cpm->cpm.generationDeltaTime = static_cast<uint16_t>(artery::countTaiMilliseconds(mTimer->getTimeFor(mVehicleDataProvider->updated())));
    cpm->cpm.cpmParameters.numberOfPerceivedObjects = numberOfPerceivedObjects;

    // Management Container
    auto& mgm = cpm->cpm.cpmParameters.managementContainer;
    // The StationType definition of the VehicleDataProvider is identical to the CPM definition
    mgm.stationType = static_cast<StationType_t>(mVehicleDataProvider->getStationType());

    if (segmented) {
        mgm.perceivedObjectContainerSegmentInfo = vanetza::asn1::allocate<PerceivedObjectContainerSegmentInfo>();
        // Use default values of a single segmented CPM in case this container gets also created for such a CPM,
        // this way the caller does not need to initialize the container
        mgm.perceivedObjectContainerSegmentInfo->totalMsgSegments = 1;
        mgm.perceivedObjectContainerSegmentInfo->thisSegmentNum = 1;
    }

    // Set the reference position like the CaService
    // It seems vehicles can leave the world and report a position out of the valid range
    const auto latitude = cp::roundToUnit(mVehicleDataProvider->latitude(), cp::units::microdecidegree);
    if (latitude >= de::latitudeMin && latitude <= de::latitudeMax) {
        mgm.referencePosition.latitude = latitude;
    } else {
        EV_ERROR << "Setting invalid latitude to unavailable: " << latitude << "\n";
        mgm.referencePosition.latitude = Latitude_unavailable;
    }
    const auto longitude = cp::roundToUnit(mVehicleDataProvider->longitude(), cp::units::microdecidegree);
    if (longitude >= de::longitudeMin && longitude <= de::longitudeMax) {
        mgm.referencePosition.longitude = longitude;
    } else {
        EV_ERROR << "Setting invalid longitude to unavailable: " << longitude << "\n";
        mgm.referencePosition.longitude = Longitude_unavailable;
    }
    mgm.referencePosition.positionConfidenceEllipse.semiMajorConfidence = SemiAxisLength_unavailable;
    mgm.referencePosition.positionConfidenceEllipse.semiMinorConfidence = SemiAxisLength_unavailable;
    mgm.referencePosition.positionConfidenceEllipse.semiMajorOrientation = HeadingValue_unavailable;
    mgm.referencePosition.altitude.altitudeValue = AltitudeValue_unavailable;
    mgm.referencePosition.altitude.altitudeConfidence = AltitudeConfidence_unavailable;

    // Station Data Container
    // This implementation only supports vehicle type stations, set values like the CaService
    cpm->cpm.cpmParameters.stationDataContainer = vanetza::asn1::allocate<StationDataContainer>();
    cpm->cpm.cpmParameters.stationDataContainer->present = StationDataContainer_PR_originatingVehicleContainer;
    auto& vehicle = cpm->cpm.cpmParameters.stationDataContainer->choice.originatingVehicleContainer;

    // Mandatory fields
    vehicle.heading.headingValue = cp::roundToUnit(mVehicleDataProvider->heading(), cp::units::decidegree);
    vehicle.heading.headingConfidence = HeadingConfidence_equalOrWithinOneDegree;
    const auto speedValue = cp::roundToUnit(abs(mVehicleDataProvider->speed()), cp::units::centimeter_per_second);
    if (speedValue >= de::speedValueMin && speedValue <= de::speedValueMax) {
        vehicle.speed.speedValue = speedValue;
    } else {
        EV_ERROR << "Setting invalid speedValue to unavailable: " << speedValue << "\n";
        vehicle.speed.speedValue = SpeedValue_unavailable;
    }
    vehicle.speed.speedConfidence = cp::roundToUnit(vanetza::units::Velocity(3.0 * cp::units::centimeter_per_second), cp::units::centimeter_per_second);
    vehicle.driveDirection = (mVehicleDataProvider->speed().value() >= 0.0 ? DriveDirection_forward : DriveDirection_backward);

    // Optional fields set from known data
    const auto longitudinalAccelerationValue = cp::convertToUnit(mVehicleDataProvider->acceleration(), cp::units::decimeter_per_second_squared);
    vehicle.longitudinalAcceleration = vanetza::asn1::allocate<LongitudinalAcceleration>();
    // Extreme speed changes can occur when SUMO swaps vehicles between lanes (speed is swapped as well)
    if (longitudinalAccelerationValue >= de::longitudinalAccelerationValueMin && longitudinalAccelerationValue <= de::longitudinalAccelerationValueMax) {
        vehicle.longitudinalAcceleration->longitudinalAccelerationValue = longitudinalAccelerationValue;
    } else {
        EV_ERROR << "Setting invalid longitudinalAccelerationValue to unavailable: " << longitudinalAccelerationValue << "\n";
        vehicle.longitudinalAcceleration->longitudinalAccelerationValue = LongitudinalAccelerationValue_unavailable;
    }
    vehicle.longitudinalAcceleration->longitudinalAccelerationConfidence = AccelerationConfidence_unavailable;
    const auto yawRateValue = cp::roundToUnit(mVehicleDataProvider->yaw_rate(), cp::units::centidegree_per_second);
    vehicle.yawRate = vanetza::asn1::allocate<YawRate>();
    if (yawRateValue >= de::yawRateValueMin && yawRateValue <= de::yawRateValueMax) {
        vehicle.yawRate->yawRateValue = yawRateValue;
    } else {
        EV_ERROR << "Setting invalid yawRateValue to unavailable: " << yawRateValue << "\n";
        vehicle.yawRate->yawRateValue = YawRateValue_unavailable;
    }
    vehicle.yawRate->yawRateConfidence = YawRateConfidence_unavailable;

    // Optional fields set by IBR implementation to increase message size to a more realistic value
    vehicle.vehicleOrientationAngle = vanetza::asn1::allocate<WGS84Angle>();
    vehicle.vehicleOrientationAngle->value = WGS84AngleValue_unavailable;
    vehicle.vehicleOrientationAngle->confidence = AngleConfidence_unavailable;
    vehicle.lateralAcceleration = vanetza::asn1::allocate<LateralAcceleration>();
    vehicle.lateralAcceleration->lateralAccelerationValue = LateralAccelerationValue_unavailable;
    vehicle.lateralAcceleration->lateralAccelerationConfidence = AccelerationConfidence_unavailable;

    return cpm;
}

cp::SensorInformationContainer_ptr CpService::createSensorInformationContainer()
{
    auto sic = cp::make_asn1<SensorInformationContainer>(asn_DEF_SensorInformationContainer);

    // WARNING: This is from the IBR implementation, a proper solution would be to extract the required information from the LEM
    auto info = cp::make_asn1<SensorInformation>(asn_DEF_SensorInformation);
    info->sensorID = 1;
    info->type = SensorType_radar;
    info->detectionArea.present = DetectionArea_PR_vehicleSensor;
    info->detectionArea.choice.vehicleSensor.refPointId = 0;
    info->detectionArea.choice.vehicleSensor.xSensorOffset = XSensorOffset_negativeZeroPointZeroOneMeter;
    info->detectionArea.choice.vehicleSensor.ySensorOffset = YSensorOffset_zeroPointZeroOneMeter;

    auto prop = cp::make_asn1<VehicleSensorProperties>(asn_DEF_VehicleSensorProperties);
    prop->range = Range_oneMeter;
    prop->horizontalOpeningAngleStart = CartesianAngleValue_oneDegree;
    prop->horizontalOpeningAngleEnd = CartesianAngleValue_oneDegree;
    cp::asn1_sequence_add(&info->detectionArea.choice.vehicleSensor.vehicleSensorPropertyList, prop);

    cp::asn1_sequence_add(sic.get(), info);

    return sic;
}

cp::PerceivedObjects CpService::createPerceivedObjects(const cp::PocCandidates& pocCandidates)
{
    cp::PerceivedObjects pos;
    pos.reserve(pocCandidates.size());
    const auto& trackedObjects = mLocalEnvironmentModel->allObjects();
    for (const auto& cand : pocCandidates) {
        const auto object = cand->first.lock();
        const auto& vehicle = object->getVehicleData();
        // Local tracking data is synchronized with the LEM so find() will always return a valid result
        const auto trackingRange = cp::aggregateTrackingTime(trackedObjects.find(cand->first)->second.sensors(), mLocalTrackingCategory);

        pos.push_back(vanetza::asn1::allocate<PerceivedObject>());
        auto& po = pos.back();

        po.objectID = vehicle.getStationId();

        // WARNING: This is from the IBR implementation, a proper solution would be to extract the required information from the LEM
        po.sensorIDList = vanetza::asn1::allocate<SensorIdList>();
        auto sensorId = cp::make_asn1<Identifier_t>(asn_DEF_Identifier);
        *sensorId = 1;
        cp::asn1_sequence_add(po.sensorIDList, sensorId);

        auto timeOfMeasurement = (mVehicleDataProvider->updated() - trackingRange.second).inUnit(opp::SIMTIME_MS);
        if (timeOfMeasurement < de::timeOfMeasurementMin) {
            EV_ERROR << "Clamping timeOfMeasurement to minimum: " << timeOfMeasurement << "\n";
            timeOfMeasurement = de::timeOfMeasurementMin;
        } else if (timeOfMeasurement > de::timeOfMeasurementMax) {
            EV_ERROR << "Clamping timeOfMeasurement to maximum: " << timeOfMeasurement << "\n";
            timeOfMeasurement = de::timeOfMeasurementMax;
        }
        po.timeOfMeasurement = timeOfMeasurement;

        auto objectAge = (trackingRange.second - trackingRange.first).inUnit(opp::SIMTIME_MS);
        if (objectAge < de::objectAgeMin) {
            EV_WARN << "Clamping objectAge to minimum: " << objectAge << "\n";
            objectAge = de::objectAgeMin;
        } else if (objectAge > de::objectAgeMax) {
            EV_INFO << "Clamping objectAge to maximum: " << objectAge << "\n";
            objectAge = de::objectAgeMax;
        }
        po.objectAge = vanetza::asn1::allocate<ObjectAge_t>();
        *po.objectAge = objectAge;

        // INFO: Currently hardcoded confidence
        // Don't use ObjectConfidence_unknown or during the calculation of priorities for segmented CPM's
        // all objects will get priority zero. Using ObjectConfidence_onePercent will result in the same priority values
        // of the IBR implementation.
        po.objectConfidence = ObjectConfidence::ObjectConfidence_onePercent;

        auto xDistanceValue = cp::roundToUnit(vehicle.position().x - mVehicleDataProvider->position().x, cp::units::centimeter);
        if (xDistanceValue < de::distanceValueMin) {
            EV_ERROR << "Clamping xDistanceValue to minimum: " << xDistanceValue;
            xDistanceValue = de::distanceValueMin;
        } else if (xDistanceValue > de::distanceValueMax) {
            EV_ERROR << "Clamping xDistanceValue to maximum: " << xDistanceValue;
            xDistanceValue = de::distanceValueMax;
        }
        po.xDistance.value = xDistanceValue;
        po.xDistance.confidence = DistanceConfidence_zeroPointZeroOneMeter;
        auto yDistanceValue = cp::roundToUnit(vehicle.position().y - mVehicleDataProvider->position().y, cp::units::centimeter);
        if (yDistanceValue < de::distanceValueMin) {
            EV_ERROR << "Clamping yDistanceValue to minimum: " << yDistanceValue;
            yDistanceValue = de::distanceValueMin;
        } else if (yDistanceValue > de::distanceValueMax) {
            EV_ERROR << "Clamping yDistanceValue to maximum: " << yDistanceValue;
            yDistanceValue = de::distanceValueMax;
        }
        po.yDistance.value = yDistanceValue;
        po.yDistance.confidence = DistanceConfidence_zeroPointZeroOneMeter;

        // WARNING: This is from the IBR implementation, a proper solution would be to decompose the speed into x- and y- direction
        //          and specify it relative to the ego vehicle speed
        const auto speedValue = cp::roundToUnit(vehicle.speed(), cp::units::centimeter_per_second);
        if (speedValue >= de::speedValueExtendedMin && speedValue <= de::speedValueExtendedMax) {
            po.xSpeed.value = speedValue;
            po.ySpeed.value = speedValue;
        } else {
            EV_ERROR << "Setting invalid speedValueExtended to unavailable: " << speedValue << "\n";
            po.xSpeed.value = SpeedValueExtended_unavailable;
            po.ySpeed.value = SpeedValueExtended_unavailable;
        }
        po.xSpeed.confidence = SpeedConfidence_equalOrWithinOneCentimeterPerSec;
        po.ySpeed.confidence = SpeedConfidence_equalOrWithinOneCentimeterPerSec;

        // INFO: Currently hardcoded values
        po.objectRefPoint = ObjectRefPoint_mid;
        po.dynamicStatus = vanetza::asn1::allocate<DynamicStatus_t>();
        *po.dynamicStatus = DynamicStatus_dynamic;

        // INFO: Currently hardcoded confidence
        const auto objectClass = cp::getObjectClass(vehicle.getStationType());
        if (objectClass.first != ObjectClass__class_PR_NOTHING) {
            po.classification = vanetza::asn1::allocate<ObjectClassDescription>();
            auto oc = cp::make_asn1<ObjectClass>(asn_DEF_ObjectClass);
            oc->confidence = ClassConfidence_oneHundredPercent;
            oc->Class.present = objectClass.first;
            switch (oc->Class.present) {
                case ObjectClass__class_PR_vehicle:
                    oc->Class.choice.vehicle.type = objectClass.second;
                    oc->Class.choice.vehicle.confidence = ClassConfidence_oneHundredPercent;
                    break;
                case ObjectClass__class_PR_person:
                    oc->Class.choice.person.type = objectClass.second;
                    oc->Class.choice.person.confidence = ClassConfidence_oneHundredPercent;
                    break;
                case ObjectClass__class_PR_animal:
                    oc->Class.choice.animal.type = objectClass.second;
                    oc->Class.choice.animal.confidence = ClassConfidence_oneHundredPercent;
                    break;
                case ObjectClass__class_PR_other:
                    oc->Class.choice.other.type = objectClass.second;
                    oc->Class.choice.other.confidence = ClassConfidence_oneHundredPercent;
                    break;
            }
            cp::asn1_sequence_add(po.classification, oc);
        }

        // Optional fields set by IBR implementation to increase message size to a more realistic value
        po.yawAngle = vanetza::asn1::allocate<CartesianAngle>();
        po.yawAngle->value = CartesianAngleValue_unavailable;
        po.yawAngle->confidence = AngleConfidence_unavailable;
        po.planarObjectDimension1 = vanetza::asn1::allocate<ObjectDimension>();
        po.planarObjectDimension1->value = ObjectDimensionValue_oneMeter;
        po.planarObjectDimension1->confidence = ObjectDimensionConfidence_oneMeter;
        po.planarObjectDimension2 = vanetza::asn1::allocate<ObjectDimension>();
        po.planarObjectDimension2->value = ObjectDimensionValue_oneMeter;
        po.planarObjectDimension2->confidence = ObjectDimensionConfidence_oneMeter;
    }

    return pos;
}


void CpService::transmitCpm(vanetza::asn1::Cpm&& cpm)
{
    vanetza::btp::DataRequestB request;
    auto port = getPortNumber(mPrimaryChannel);
    request.destination_port = vanetza::host_cast<artery::PortNumber>(port);
    request.gn.its_aid = vanetza::aid::CP;
    request.gn.transport_type = vanetza::geonet::TransportType::SHB;
    request.gn.maximum_lifetime = vanetza::geonet::Lifetime{vanetza::geonet::Lifetime::Base::One_Second, 1};
    request.gn.communication_profile = vanetza::geonet::CommunicationProfile::ITS_G5;

    // This is the reverse from what is done during initialize()
    request.gn.traffic_class.tc_id(static_cast<unsigned int>(mDccProfile));

    CpObject obj(std::move(cpm));
    emit(evt::cpmSent, &obj);

    using CpmByteBuffer = vanetza::convertible::byte_buffer_impl<vanetza::asn1::Cpm>;
    std::unique_ptr<vanetza::geonet::DownPacket> payload{new vanetza::geonet::DownPacket()};
    std::unique_ptr<vanetza::convertible::byte_buffer> buffer{new CpmByteBuffer(obj.shared_ptr())};
    payload->layer(vanetza::OsiLayer::Application) = std::move(buffer);
    this->request(request, std::move(payload));
}


double CpService::getCurrentCbr() const
{
    auto ifc = mNetworkInterfaceTable->select(mPrimaryChannel);
    if (!ifc) {
        throw opp::cRuntimeError("No NetworkInterface found for CP's primary channel %i", mPrimaryChannel);
    }
    auto* scpp = dynamic_cast<vanetza::dcc::SmoothingChannelProbeProcessor*>(ifc->getDccEntity().getChannelProbeProcessor());
    if (!scpp) {
        throw opp::cRuntimeError("No SmoothingChannelProbeProcessor found for CP's primary channel %i", mPrimaryChannel);
    }

    return scpp->channel_load().value();
}

omnetpp::SimTime CpService::generateCpmDcc()
{
    // This is copied from CaService
    auto ifc = mNetworkInterfaceTable->select(mPrimaryChannel);
    auto* trc = (ifc ? ifc->getDccEntity().getTransmitRateThrottle() : nullptr);
    if (!trc) {
        throw opp::cRuntimeError("No DCC TRC found for CP's primary channel %i", mPrimaryChannel);
    }

    static const vanetza::dcc::TransmissionLite ca_tx(mDccProfile, 0);
    auto interval = trc->interval(ca_tx);
    opp::SimTime dcc(std::chrono::duration_cast<std::chrono::milliseconds>(interval).count(), opp::SIMTIME_MS);

    return std::min(mGenCpmMax, std::max(mGenCpmMin, dcc));
}


void CpService::emitRemoteObjectStatistics(
    const cp::RemoteDynamics& object, const boost::optional<cp::CpmDynamics> prevDynamics, const cp::CpmDynamics& currentDynamics)
{
    if (prevDynamics) {
        emit(sc::cpmRecv_timeSinceLastObjectUpdate, static_cast<long>((currentDynamics.received - prevDynamics->received).inUnit(opp::SIMTIME_MS)));
        emit(
            sc::cpmRecv_distanceSinceLastObjectUpdate,
            cp::convertToUnit(artery::distance(currentDynamics.trackedDynamics.position, prevDynamics->trackedDynamics.position), vanetza::units::si::meter));
    }
    emit(sc::cpmRecv_senderRedundancy, static_cast<unsigned long>(object.getCpmDynamics().size()));
    emit(sc::cpmRecv_poRedundancy, static_cast<unsigned long>(object.getCpmDynamicsCount()));
}

}  // namespace artery
