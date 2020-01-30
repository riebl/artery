#include "artery/ros/ProxyService.h"
#include "artery/application/VehicleDataProvider.h"
#include "artery/utility/Identity.h"
#include <etsi_its_msgs/CAM.h>
#include <sensor_msgs/NavSatFix.h>
#include <regex>

namespace artery
{

Define_Module(RosProxyService)

using namespace omnetpp;
static const omnetpp::simsignal_t scSignalCamReceived = cComponent::registerSignal("CamReceived");

namespace
{

inline uint8_t reverse_byte(uint8_t byte)
{
    byte = (byte & 0xf0) >> 4 | (byte & 0x0f) << 4;
    byte = (byte & 0xcc) >> 2 | (byte & 0x33) << 2;
    byte = (byte & 0xaa) >> 1 | (byte & 0x55) << 1;
    return byte;
}

} // namespace


void RosProxyService::initialize()
{
    ItsG5PromiscuousService::initialize();
    mVehicleDataProvider = getFacilities().get_const_ptr<VehicleDataProvider>();
    subscribe(scSignalCamReceived);

    std::string ns = par("namespace");

    // replace %SUMO_ID% placeholder by actual ID maintained by SUMO
    const Identity& identity = getFacilities().get_const<Identity>();
    ns = std::regex_replace(ns, std::regex("%SUMO_ID%"), identity.traci);

    // replace characters not allowed in ROS names
    std::regex ros_name("[^a-zA-Z0-9/]");
    ns = std::regex_replace(ns, ros_name, "_");

    ros::NodeHandle nh(ns);
    mNavSatPublisher = nh.advertise<sensor_msgs::NavSatFix>("nav_sat_fix", 10);
    mCamRxPublisher = nh.advertise<etsi_its_msgs::CAM>("cam_rx", 10);
}

void RosProxyService::trigger()
{
    Enter_Method("trigger");
    if (mVehicleDataProvider) {
        sensor_msgs::NavSatFix fix;
        fix.header.stamp = ros::Time::now();
        fix.status.status = sensor_msgs::NavSatStatus::STATUS_FIX;
        fix.status.service = sensor_msgs::NavSatStatus::SERVICE_GPS;
        fix.latitude = mVehicleDataProvider->latitude() / vanetza::units::degree;
        fix.longitude = mVehicleDataProvider->longitude() / vanetza::units::degree;
        fix.position_covariance_type = sensor_msgs::NavSatFix::COVARIANCE_TYPE_UNKNOWN;
        mNavSatPublisher.publish(fix);
    }
}

void RosProxyService::receiveSignal(cComponent* source, simsignal_t signal, cObject* obj, cObject*)
{
    if (signal == scSignalCamReceived) {
        auto* cam = dynamic_cast<CaObject*>(obj);
        if (cam) {
            publishReceivedCam(*cam);
        } else {
            EV_ERROR << "CAM reception signal does not carry CaObject\n";
        }
    }
}

void RosProxyService::publishReceivedCam(const CaObject& obj)
{
    auto asn1 = obj.asn1();
    auto msg = boost::make_shared<etsi_its_msgs::CAM>();

    // etsi_its_msgs/CAM header fields
    msg->header.stamp = ros::Time::now();
    msg->its_header.protocol_version = asn1->header.protocolVersion;
    msg->its_header.station_id = asn1->header.stationID;
    msg->generation_delta_time = asn1->cam.generationDeltaTime;

    // basic container
    const auto& params = asn1->cam.camParameters;
    msg->station_type.value = params.basicContainer.stationType;
    const auto& refpos = params.basicContainer.referencePosition;
    msg->reference_position.altitude.value = refpos.altitude.altitudeValue;
    msg->reference_position.altitude.confidence = refpos.altitude.altitudeConfidence;
    msg->reference_position.latitude = refpos.latitude;
    msg->reference_position.longitude = refpos.longitude;
    msg->reference_position.position_confidence.semi_major_confidence = refpos.positionConfidenceEllipse.semiMajorConfidence;
    msg->reference_position.position_confidence.semi_minor_confidence = refpos.positionConfidenceEllipse.semiMinorConfidence;
    msg->reference_position.position_confidence.semi_major_orientation = refpos.positionConfidenceEllipse.semiMajorOrientation;

    // high frequency container
    if (params.highFrequencyContainer.present == HighFrequencyContainer_PR_basicVehicleContainerHighFrequency) {
        const auto& hfc = params.highFrequencyContainer.choice.basicVehicleContainerHighFrequency;
        msg->high_frequency_container.heading.value = hfc.heading.headingValue;
        msg->high_frequency_container.heading.confidence = hfc.heading.headingConfidence;
        msg->high_frequency_container.speed.value = hfc.speed.speedValue;
        msg->high_frequency_container.speed.confidence = hfc.speed.speedConfidence;
        msg->high_frequency_container.drive_direction.value = hfc.driveDirection;
        msg->high_frequency_container.vehicle_length.value = hfc.vehicleLength.vehicleLengthValue;
        msg->high_frequency_container.vehicle_length.confidence_indication = hfc.vehicleLength.vehicleLengthConfidenceIndication;
        msg->high_frequency_container.vehicle_width.value = hfc.vehicleWidth;
        msg->high_frequency_container.longitudinal_acceleration.value = hfc.longitudinalAcceleration.longitudinalAccelerationValue;
        msg->high_frequency_container.longitudinal_acceleration.confidence = hfc.longitudinalAcceleration.longitudinalAccelerationConfidence;
        msg->high_frequency_container.curvature.value = hfc.curvature.curvatureValue;
        msg->high_frequency_container.curvature.confidence = hfc.curvature.curvatureConfidence;
        msg->high_frequency_container.curvature_calculation_mode.value = hfc.curvatureCalculationMode;
        msg->high_frequency_container.yaw_rate.value = hfc.yawRate.yawRateValue;
        msg->high_frequency_container.yaw_rate.confidence = hfc.yawRate.yawRateConfidence;
    } else {
        EV_ERROR << "missing BasicVehicleContainerHighFrequency container\n";
        return;
    }

    if (params.lowFrequencyContainer && params.lowFrequencyContainer->present == LowFrequencyContainer_PR_basicVehicleContainerLowFrequency) {
        const auto& lfc = params.lowFrequencyContainer->choice.basicVehicleContainerLowFrequency;
        msg->has_low_frequency_container = true;

        msg->low_frequency_container.vehicle_role.value = lfc.vehicleRole;
        msg->low_frequency_container.exterior_lights.value = 0;
        if (lfc.exteriorLights.size == 1) {
            // need to reverse bits from asn1c bit string to match our bit masks
            msg->low_frequency_container.exterior_lights.value = reverse_byte(lfc.exteriorLights.buf[0]);
        }

        for (int i = 0; i < lfc.pathHistory.list.count; ++i) {
            const PathPoint_t* asn1_path_point = lfc.pathHistory.list.array[i];
            etsi_its_msgs::PathPoint path_point;

            path_point.path_position.delta_latitude = asn1_path_point->pathPosition.deltaLatitude;
            path_point.path_position.delta_longitude = asn1_path_point->pathPosition.deltaLongitude;
            path_point.path_position.delta_altitude = asn1_path_point->pathPosition.deltaAltitude;

            path_point.path_delta_time.value = etsi_its_msgs::PathDeltaTime::UNAVAILABLE;
            if (asn1_path_point->pathDeltaTime) {
                path_point.path_delta_time.value = *(asn1_path_point->pathDeltaTime);
            }

            msg->low_frequency_container.path_history.points.push_back(path_point);
        }
    }

    mCamRxPublisher.publish(msg);
}

} // namespace artery
