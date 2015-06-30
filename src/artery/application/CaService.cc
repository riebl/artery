#include "artery/application/CaService.h"
#include "artery/application/VehicleDataProvider.h"
#include "veins/base/utils/Coord.h"
#undef ev
#include <vanetza/btp/ports.hpp>
#include <boost/units/cmath.hpp>
#include <boost/units/systems/si/prefixes.hpp>
#include <boost/variant/static_visitor.hpp>

auto microdegree = vanetza::units::degree * boost::units::si::micro;
auto decidegree = vanetza::units::degree * boost::units::si::deci;
auto degree_per_second = vanetza::units::degree / vanetza::units::si::second;
auto centimeter_per_second = vanetza::units::si::meter_per_second * boost::units::si::centi;

static const simsignal_t scSignalCamReceived = cComponent::registerSignal("CaService.received");
static const simsignal_t scSignalCamSent = cComponent::registerSignal("CaService.sent");

Define_Module(CaService);


bool checkHeadingDelta(vanetza::units::Angle& prev, vanetza::units::Angle now)
{
	static const vanetza::units::Angle scHeadingDelta { 4.0 * vanetza::units::degree };
	return abs(prev - now) > scHeadingDelta;
}

bool checkPositionDelta(const Coord& prev, const Coord& now)
{
	static const double scPositionDeltaMeter = 4.0;
	return (prev.distance(now) > scPositionDeltaMeter);
}

bool checkSpeedDelta(vanetza::units::Velocity prev, vanetza::units::Velocity now)
{
	static const vanetza::units::Velocity scSpeedDelta = 0.5 * vanetza::units::si::meter_per_second;
	return abs(prev - now) > scSpeedDelta;
}


CaService::CaService() :
		mGenCamMin { 100, SIMTIME_MS },
		mGenCamMax { 1000, SIMTIME_MS },
		mGenCam(mGenCamMax),
		mGenCamLowDynamicsCounter(0),
		mGenCamLowDynamicsLimit(3)
{
}

void CaService::trigger()
{
	checkTriggeringConditions(getFacilities().getVehicleDataProvider(), simTime());
}

void CaService::indicate(const vanetza::btp::DataIndication& ind, std::unique_ptr<vanetza::btp::UpPacket> packet)
{
	using namespace vanetza;

	struct packet_visitor : public boost::static_visitor<asn1::Cam*>
	{
		asn1::Cam* operator()(CohesivePacket& packet)
		{
			opp_error("CAM deserialization isn't implemented yet in CaService");
			return nullptr;
		}

		asn1::Cam* operator()(ChunkPacket& packet)
		{
			typedef convertible::byte_buffer byte_buffer;
			typedef convertible::byte_buffer_impl<asn1::Cam> byte_buffer_impl;

			byte_buffer* ptr = packet[OsiLayer::Application].ptr();
			auto impl = dynamic_cast<byte_buffer_impl*>(ptr);
			if (impl) {
				return &(impl->m_wrapper);
			} else {
				opp_error("ChunkPacket doesn't contain a CAM structure");
				return nullptr;
			}
		}
	};

	packet_visitor visitor;
	asn1::Cam* cam = boost::apply_visitor(visitor, *packet);
	if (cam) {
		// TODO: collect statistic data
		emit(scSignalCamReceived, cam->validate());
	}
}

void CaService::checkTriggeringConditions(const VehicleDataProvider& vdp, const simtime_t& T_now)
{
	simtime_t& T_GenCam = mGenCam;
	const simtime_t& T_GenCamMin = mGenCamMin;
	const simtime_t& T_GenCamMax = mGenCamMax;
	const simtime_t T_GenCamDcc = genCamDcc();
	const simtime_t T_elapsed = T_now - mLastCamTimestamp;

	if (T_GenCamDcc < T_GenCamMin || T_GenCamDcc > T_GenCamMax) {
		opp_error("T_GenCamDcc is out of bounds");
	}

	if (T_elapsed >= T_GenCamDcc) {
		if (checkHeadingDelta(mLastCamHeading, vdp.heading()) ||
			checkPositionDelta(mLastCamPosition, vdp.position()) ||
			checkSpeedDelta(mLastCamSpeed, vdp.speed())) {
			sendCam(vdp, T_now);
			T_GenCam = T_elapsed;
			mGenCamLowDynamicsCounter = 0;
		} else if (T_elapsed >= T_GenCam) {
			sendCam(vdp, T_now);
			if (++mGenCamLowDynamicsCounter >= mGenCamLowDynamicsLimit) {
				T_GenCam = T_GenCamMax;
			}
		}
	}
}

void CaService::sendCam(const VehicleDataProvider& vdp, const simtime_t& T_now)
{
	auto cam = createCooperativeAwarenessMessage(vdp);
	mLastCamPosition = vdp.position();
	mLastCamSpeed = vdp.speed();
	mLastCamHeading = vdp.heading();
	mLastCamTimestamp = T_now;
	if (T_now - mLastLowCamTimestamp >= simtime_t { 500, SIMTIME_MS }) {
		addLowFrequencyContainer(cam);
		mLastLowCamTimestamp = T_now;
	}

	using namespace vanetza;
	btp::DataRequestB request;
	request.destination_port = btp::ports::CAM;
	request.gn.transport_type = geonet::TransportType::SHB;
	request.gn.traffic_class.tc_id(static_cast<unsigned>(dcc::Profile::DP2));
	request.gn.communication_profile = geonet::CommunicationProfile::ITS_G5;

	std::unique_ptr<geonet::DownPacket> payload { new geonet::DownPacket };
	payload->layer(OsiLayer::Application) = std::move(cam);
	const std::size_t payload_length = payload->size();
	this->request(request, std::move(payload));

	emit(scSignalCamSent, payload_length);
}

simtime_t CaService::genCamDcc()
{
	vanetza::clock::duration delay = getFacilities().getDccScheduler().delay(vanetza::dcc::Profile::DP2);
	simtime_t dcc { static_cast<int64_t>(delay.count()), SIMTIME_MS };
	return std::max(mGenCamMin, dcc);
}

vanetza::asn1::Cam createCooperativeAwarenessMessage(const VehicleDataProvider& vdp)
{
	vanetza::asn1::Cam message;

	ItsPduHeader_t& header = (*message).header;
	header.protocolVersion = ItsPduHeader__protocolVersion_currentVersion;
	header.messageID = ItsPduHeader__messageID_cam;
	header.stationID = vdp.station_id();

	CoopAwareness_t& cam = (*message).cam;
	cam.generationDeltaTime = vdp.timestamp() * GenerationDeltaTime_oneMilliSec;
	BasicContainer_t& basic = cam.camParameters.basicContainer;
	HighFrequencyContainer_t& hfc = cam.camParameters.highFrequencyContainer;

	basic.stationType = StationType_passengerCar;
	basic.referencePosition.altitude.altitudeValue = AltitudeValue_seaLevel;
	basic.referencePosition.altitude.altitudeConfidence = AltitudeConfidence_unavailable;
	basic.referencePosition.longitude = (vdp.longitude() / microdegree).value() * Longitude_oneMicrodegreeEast;
	basic.referencePosition.latitude = (vdp.latitude() / microdegree).value() * Latitude_oneMicrodegreeNorth;
	basic.referencePosition.positionConfidenceEllipse.semiMajorOrientation.
	headingValue = HeadingValue_unavailable;
	basic.referencePosition.positionConfidenceEllipse.semiMajorOrientation.
	headingConfidence = HeadingConfidence_unavailable;
	basic.referencePosition.positionConfidenceEllipse.semiMajorConfidence =
			SemiAxisLength_unavailable;
	basic.referencePosition.positionConfidenceEllipse.semiMinorConfidence =
			SemiAxisLength_unavailable;

	hfc.present = HighFrequencyContainer_PR_basicVehicleContainerHighFrequency;
	BasicVehicleContainerHighFrequency& bvc = hfc.choice.basicVehicleContainerHighFrequency;
	bvc.heading.headingValue = (vdp.heading() / decidegree).value();
	bvc.heading.headingConfidence = HeadingConfidence_withinOneDegree;
	bvc.speed.speedValue = std::abs((vdp.speed() / centimeter_per_second).value()) *
			SpeedValue_oneCentimeterPerSec;
	bvc.speed.speedConfidence = SpeedConfidence_withinOneCentimeterPerSec * 3;
	bvc.driveDirection = vdp.speed().value() >= 0.0 ?
			DriveDirection_forward : DriveDirection_backward;
	bvc.longitudinalAcceleration.longitudinalAccelerationValue =
			(vdp.acceleration() / vanetza::units::si::meter_per_second_squared).value() * LongitudinalAccelerationValue_pointOneMeterPerSecSquaredForward;
	bvc.longitudinalAcceleration.longitudinalAccelerationConfidence =
			AccelerationConfidence_unavailable;
	bvc.curvature.curvatureValue = (vdp.curvature() / vanetza::units::reciprocal_metre) *
			CurvatureValue_reciprocalOf1MeterRadiusToLeft;
	bvc.curvature.curvatureConfidence = CurvatureConfidence_unavailable;
	bvc.curvatureCalculationMode = CurvatureCalculationMode_yawRateUsed;
	bvc.yawRate.yawRateValue = (vdp.yaw_rate() / degree_per_second).value() *
			YawRateValue_degSec_000_01ToLeft * 100.0;
	bvc.vehicleLength.vehicleLengthValue = VehicleLengthValue_unavailable;
	bvc.vehicleLength.vehicleLengthConfidenceIndication =
			VehicleLengthConfidenceIndication_noTrailerPresent;
	bvc.vehicleWidth = VehicleWidth_unavailable;

	std::string error;
	if (!message.validate(error)) {
		opp_error("Invalid High Frequency CAM: %s", error.c_str());
	}

	return message;
}

void addLowFrequencyContainer(vanetza::asn1::Cam& message)
{
	LowFrequencyContainer_t*& lfc = message->cam.camParameters.lowFrequencyContainer;
	lfc = vanetza::asn1::allocate<LowFrequencyContainer_t>();
	lfc->present = LowFrequencyContainer_PR_basicVehicleContainerLowFrequency;
	BasicVehicleContainerLowFrequency& bvc = lfc->choice.basicVehicleContainerLowFrequency;
	bvc.vehicleRole = VehicleRole_default;
	bvc.exteriorLights.buf = static_cast<uint8_t*>(malloc(1));
	assert(nullptr != bvc.exteriorLights.buf);
	bvc.exteriorLights.size = 1;
	bvc.exteriorLights.buf[0] |= 1 << (7 - ExteriorLights_daytimeRunningLightsOn);
	// TODO: add pathHistory

	std::string error;
	if (!message.validate(error)) {
		opp_error("Invalid Low Frequency CAM: %s", error.c_str());
	}
}
