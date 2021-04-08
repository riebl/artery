//
// Copyright (C) 2015 Raphael Riebl <raphael.riebl@thi.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "artery/transfusion/TransfusionService.h"
#include "artery/utility/AsioScheduler.h"
#include "TransfusionMsg.pb.h"
#include <boost/units/systems/si/prefixes.hpp>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <omnetpp/csimulation.h>
#include <vanetza/common/byte_view.hpp>
#include <sstream>

using namespace omnetpp;

namespace artery
{

Define_Module(TransfusionService)

void TransfusionService::initialize()
{
    ItsG5BaseService::initialize();
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    AsioScheduler* scheduler = check_and_cast<AsioScheduler*>(getSimulation()->getScheduler());
    m_asio_task = scheduler->createTask(*this);

    boost::asio::ip::tcp::endpoint endpoint;
    boost::system::error_code ec;
    auto remote_ip = boost::asio::ip::address_v4::from_string(par("remote_ip").stringValue(), ec);
    if (ec) {
        throw cRuntimeError("remote IP address is invalid: %s", par("remote_ip").stringValue());
    }
    unsigned remote_port = par("remote_port");
    endpoint.address(remote_ip);
    endpoint.port(remote_port);

    try {
        m_asio_task->connect(endpoint, par("tcp_no_delay").boolValue());
    } catch (boost::system::system_error& e) {
        throw cRuntimeError("TransfusionService: %s", e.what());
    }
}

void TransfusionService::handleMessage(cMessage* msg)
{
    if (msg == m_asio_task->getDataMessage())
    {
        // receiving a message from external software
        auto buffer = m_asio_task->getDataMessage()->getBuffer();
        std::size_t len = m_asio_task->getDataMessage()->getLength();
        std::copy(buffer.data(), buffer.data() + len, std::back_inserter(m_buffer));

        google::protobuf::io::ArrayInputStream aos(buffer.data(), buffer.size());
        google::protobuf::io::CodedInputStream cis(&aos);
        uint32_t msg_length = 0;
        cis.ReadVarint32(&msg_length);
        if (msg_length > 0 && m_buffer.size() >= msg_length) {
            auto limit = cis.PushLimit(msg_length);
            Transfusion::TransfusionMsg msg;
            if (msg.ParseFromCodedStream(&cis)) {
                processMessage(msg);
            } else {
                EV_WARN << "Decoding of Transfusion message failed, skip it";
            }
            cis.PopLimit(limit);
            m_buffer.erase(m_buffer.begin(), m_buffer.begin() + cis.CurrentPosition());
        }

        // signal scheduler that we are ready to handle further data
        m_asio_task->handleNext();
    }
}

void TransfusionService::processMessage(const Transfusion::TransfusionMsg& msg)
{
    using namespace vanetza;
    btp::DataRequestB request;
    request.destination_port = host_cast<uint16_t>(msg.destination_port());
    switch (msg.transport_type_case())
    {
        case Transfusion::TransfusionMsg::kShb:
            request.gn.transport_type = geonet::TransportType::SHB;
            break;
        case Transfusion::TransfusionMsg::kGbc:
            request.gn.transport_type = geonet::TransportType::GBC;
            request.gn.destination = buildDestinationArea(msg.gbc());
            break;
        default:
            EV_WARN << "unknown transport type of received Transfusion message, skip it";
            return;
            break;
    }

    if (msg.has_maximum_lifetime()) {
        request.gn.maximum_lifetime = geonet::Lifetime();
        request.gn.maximum_lifetime->encode(msg.maximum_lifetime() * units::si::seconds);
    }

    request.gn.traffic_class.tc_id(msg.traffic_class().dcc_profile());
    request.gn.traffic_class.channel_offload(msg.traffic_class().channel_offload());
    request.gn.traffic_class.store_carry_forward(msg.traffic_class().store_carry_forward());
    request.gn.communication_profile = geonet::CommunicationProfile::ITS_G5;

    std::unique_ptr<geonet::DownPacket> payload { new geonet::DownPacket };
    payload->layer(OsiLayer::Application) = std::move(msg.payload());
    this->request(request, std::move(payload));
}

void TransfusionService::tapPacket(const vanetza::btp::DataIndication& indication, const vanetza::UpPacket& packet)
{
    using namespace Transfusion;
    using namespace vanetza;

    TransfusionMsg msg;
    msg.set_destination_port(indication.destination_port.host());
    const auto payload = create_byte_view(packet, OsiLayer::Application);
    msg.set_payload(payload.data(), payload.size());

    TrafficClass* traffic_class = msg.mutable_traffic_class();
    traffic_class->set_dcc_profile(static_cast<TrafficClass::DccProfile>(indication.traffic_class.tc_id().raw()));
    traffic_class->set_channel_offload(indication.traffic_class.channel_offload());
    traffic_class->set_store_carry_forward(indication.traffic_class.store_carry_forward());

    if (indication.remaining_packet_lifetime) {
        using vanetza::units::si::seconds;
        msg.set_maximum_lifetime(indication.remaining_packet_lifetime->decode() / seconds);
    }

    std::ostringstream strstream;
    {
        google::protobuf::io::OstreamOutputStream os(&strstream);
        google::protobuf::io::CodedOutputStream cos(&os);
        cos.WriteVarint32(msg.ByteSizeLong());
        if(!msg.SerializeToCodedStream(&cos)) {
            throw cRuntimeError("Encoding of Transfusion message failed");
        }
    }
    m_asio_task->write(boost::asio::buffer(strstream.str()));
}

vanetza::geonet::Area TransfusionService::buildDestinationArea(const Transfusion::GeoBroadcast& gbc)
{
    using vanetza::units::si::meters;
    static const auto microdegree = vanetza::units::si::micro * vanetza::units::degree;

    vanetza::geonet::Area area;
    switch (gbc.area().shape()) {
        case Transfusion::DestinationArea::Rectangle:
            area.shape = vanetza::geonet::Rectangle();
            boost::get<vanetza::geonet::Rectangle>(area.shape).a = gbc.area().distance_a() * meters;
            boost::get<vanetza::geonet::Rectangle>(area.shape).b = gbc.area().distance_b() * meters;
            break;
        case Transfusion::DestinationArea::Ellipse:
            area.shape = vanetza::geonet::Ellipse();
            boost::get<vanetza::geonet::Ellipse>(area.shape).a = gbc.area().distance_a() * meters;
            boost::get<vanetza::geonet::Ellipse>(area.shape).b = gbc.area().distance_b() * meters;
            break;
        default:
            EV_WARN << "Unknown shape type for destination area, fall back to circle";
        case Transfusion::DestinationArea::Circle:
            area.shape = vanetza::geonet::Circle();
            boost::get<vanetza::geonet::Circle>(area.shape).r = gbc.area().distance_a() * meters;
            break;
    }

    area.angle = vanetza::units::Angle { gbc.area().angle() * vanetza::units::degrees };
    area.position.latitude = vanetza::units::GeoAngle { gbc.area().latitude() * 0.1 * microdegree };
    area.position.longitude = vanetza::units::GeoAngle { gbc.area().longitude() * 0.1 * microdegree };

    return area;
}

} // namespace artery
