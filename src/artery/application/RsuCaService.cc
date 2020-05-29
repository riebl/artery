/*
* Artery V2X Simulation Framework
* Copyright 2014-2020 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/application/CaObject.h"
#include "artery/application/LocalDynamicMap.h"
#include "artery/application/RsuCaService.h"
#include "artery/application/Asn1PacketVisitor.h"
#include "artery/application/MultiChannelPolicy.h"
#include "artery/utility/Geometry.h"
#include "artery/utility/Identity.h"
#include <boost/lexical_cast.hpp>
#include <omnetpp/cexception.h>
#include <omnetpp/cxmlelement.h>
#include <vanetza/btp/ports.hpp>
#include <cmath>

namespace artery
{

using namespace omnetpp;

static const simsignal_t scSignalCamReceived = cComponent::registerSignal("CamReceived");
static const simsignal_t scSignalCamSent = cComponent::registerSignal("CamSent");

Define_Module(RsuCaService)

void RsuCaService::initialize()
{
    ItsG5BaseService::initialize();
    mIdentity = &getFacilities().get_const<Identity>();
    mGeoPosition = &getFacilities().get_const<GeoPosition>();
    mNetworkInterfaceTable = &getFacilities().get_const<NetworkInterfaceTable>();
    mTimer = &getFacilities().get_const<Timer>();
    mLocalDynamicMap = &getFacilities().get_mutable<LocalDynamicMap>();

    mGenerationInterval = par("generationInterval");
    mLastCamTimestamp = -mGenerationInterval;
    mProtectedCommunicationZones = parseProtectedCommunicationZones(par("protectedCommunicationZones").xmlValue());
    if (mProtectedCommunicationZones.size() > 16) {
        throw cRuntimeError("CAMs can include at most 16 protected communication zones");
    } else {
        EV_INFO << "announcing " << mProtectedCommunicationZones.size() << " protected communication zones\n";
    }

    // look up primary channel for CA
    mPrimaryChannel = getFacilities().get_const<MultiChannelPolicy>().primaryChannel(vanetza::aid::CA);
}

auto RsuCaService::parseProtectedCommunicationZones(cXMLElement* zones_cfg) -> std::list<ProtectedCommunicationZone>
{
    std::list<ProtectedCommunicationZone> zones;

    for (cXMLElement* zone_cfg : zones_cfg->getChildrenByTagName("zone")) {
        ProtectedCommunicationZone zone;
        zone.latitude_deg = boost::lexical_cast<double>(zone_cfg->getAttribute("latitude"));
        zone.longitude_deg = boost::lexical_cast<double>(zone_cfg->getAttribute("longitude"));
        const char* radius_attr = zone_cfg->getAttribute("radius");
        if (radius_attr) {
            zone.radius_m = boost::lexical_cast<unsigned>(radius_attr);
        }
        const char* id_attr = zone_cfg->getAttribute("id");
        if (id_attr) {
            zone.id = boost::lexical_cast<unsigned>(id_attr);
        }
        const char* type_attr = zone_cfg->getAttribute("type");
        if (type_attr) {
            if (std::strcmp(type_attr, "permanent") == 0) {
                zone.type = ProtectedZoneType_permanentCenDsrcTolling;
            } else if (std::strcmp(type_attr, "temporary") == 0) {
                zone.type = ProtectedZoneType_temporaryCenDsrcTolling;
            } else {
                zone.type = boost::lexical_cast<ProtectedZoneType_t>(type_attr);
            }
        }
        zones.push_back(zone);
    }

    return zones;
}

void RsuCaService::trigger()
{
    Enter_Method("trigger");
    if (simTime() - mLastCamTimestamp >= mGenerationInterval) {
        sendCam();
    }
}

void RsuCaService::indicate(const vanetza::btp::DataIndication& ind, std::unique_ptr<vanetza::UpPacket> packet)
{
    Enter_Method("indicate");

    Asn1PacketVisitor<vanetza::asn1::Cam> visitor;
    const vanetza::asn1::Cam* cam = boost::apply_visitor(visitor, *packet);
    if (cam && cam->validate()) {
        CaObject obj = visitor.shared_wrapper;
        emit(scSignalCamReceived, &obj);
        mLocalDynamicMap->updateAwareness(obj);
    }
}

void RsuCaService::sendCam()
{
    using namespace vanetza;
    btp::DataRequestB request;
    request.destination_port = btp::ports::CAM;
    request.gn.its_aid = aid::CA;
    request.gn.transport_type = geonet::TransportType::SHB;
    request.gn.maximum_lifetime = geonet::Lifetime { geonet::Lifetime::Base::One_Second, 1 };
    request.gn.traffic_class.tc_id(static_cast<unsigned>(dcc::Profile::DP2));
    request.gn.communication_profile = geonet::CommunicationProfile::ITS_G5;

    CaObject obj(createMessage());
    emit(scSignalCamSent, &obj);

    using CamByteBuffer = convertible::byte_buffer_impl<asn1::Cam>;
    std::unique_ptr<geonet::DownPacket> payload { new geonet::DownPacket() };
    std::unique_ptr<convertible::byte_buffer> buffer { new CamByteBuffer(obj.shared_ptr()) };
    payload->layer(OsiLayer::Application) = std::move(buffer);
    this->request(request, std::move(payload));
}

vanetza::asn1::Cam RsuCaService::createMessage() const
{
    vanetza::asn1::Cam message;
    ItsPduHeader_t& header = (*message).header;
    header.protocolVersion = 2;
    header.messageID = ItsPduHeader__messageID_cam;
    header.stationID = mIdentity->application;

    CoopAwareness_t& cam = (*message).cam;
    const uint16_t genDeltaTime = countTaiMilliseconds(mTimer->getCurrentTime());
    cam.generationDeltaTime = genDeltaTime * GenerationDeltaTime_oneMilliSec;
    BasicContainer_t& basic = cam.camParameters.basicContainer;
    HighFrequencyContainer_t& hfc = cam.camParameters.highFrequencyContainer;

    basic.stationType = StationType_roadSideUnit;
    basic.referencePosition.altitude.altitudeValue = AltitudeValue_unavailable;
    basic.referencePosition.altitude.altitudeConfidence = AltitudeConfidence_unavailable;
    const double longitude = mGeoPosition->longitude / vanetza::units::degree;
    basic.referencePosition.longitude = std::round(longitude * 1e6 * Longitude_oneMicrodegreeEast);
    const double latitude = mGeoPosition->latitude / vanetza::units::degree;
    basic.referencePosition.latitude = std::round(latitude * 1e6 * Latitude_oneMicrodegreeNorth);
    basic.referencePosition.positionConfidenceEllipse.semiMajorOrientation = HeadingValue_unavailable;
    basic.referencePosition.positionConfidenceEllipse.semiMajorConfidence = SemiAxisLength_unavailable;
    basic.referencePosition.positionConfidenceEllipse.semiMinorConfidence = SemiAxisLength_unavailable;

    hfc.present = HighFrequencyContainer_PR_rsuContainerHighFrequency;
    RSUContainerHighFrequency& rchf = hfc.choice.rsuContainerHighFrequency;
    if (!mProtectedCommunicationZones.empty()) {
        rchf.protectedCommunicationZonesRSU = vanetza::asn1::allocate<ProtectedCommunicationZonesRSU_t>();
        for (const ProtectedCommunicationZone& zone : mProtectedCommunicationZones) {
            auto asn1 = vanetza::asn1::allocate<ProtectedCommunicationZone_t>();
            asn1->protectedZoneType = zone.type;
            asn1->protectedZoneLatitude = std::round(zone.latitude_deg * 1e6 * Latitude_oneMicrodegreeNorth);
            asn1->protectedZoneLongitude = std::round(zone.longitude_deg * 1e6 * Longitude_oneMicrodegreeEast);
            if (zone.radius_m > 0) {
                asn1->protectedZoneRadius = vanetza::asn1::allocate<ProtectedZoneRadius_t>();
                *asn1->protectedZoneRadius = zone.radius_m;
            }
            if (zone.id) {
                asn1->protectedZoneID = vanetza::asn1::allocate<ProtectedZoneID_t>();
                *asn1->protectedZoneID = *zone.id;
            }
            ASN_SEQUENCE_ADD(rchf.protectedCommunicationZonesRSU, asn1);
        }
    }

    std::string error;
    if (!message.validate(error)) {
            throw cRuntimeError("Invalid RSU CAM: %s", error.c_str());
    }

    return message;
}

} // namespace artery
