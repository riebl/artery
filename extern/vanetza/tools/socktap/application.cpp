#include "application.hpp"
#include <vanetza/btp/header.hpp>
#include <vanetza/btp/header_conversion.hpp>
#include <cassert>

using namespace vanetza;

Application::DataConfirm Application::request(const DataRequest& request, DownPacketPtr packet)
{
    DataConfirm confirm(DataConfirm::ResultCode::REJECTED_UNSPECIFIED);
    if (router_ && packet) {
        btp::HeaderB btp_header;
        btp_header.destination_port = this->port();
        btp_header.destination_port_info = host_cast<uint16_t>(0);
        packet->layer(OsiLayer::Transport) = btp_header;

        switch (request.transport_type) {
            case geonet::TransportType::SHB:
                confirm = router_->request(request_shb(request), std::move(packet));
                break;
            case geonet::TransportType::GBC:
                confirm = router_->request(request_gbc(request), std::move(packet));
                break;
            default:
                // TODO remaining transport types are not implemented
                break;
        }
    }

    return confirm;
}

void initialize_request(const Application::DataRequest& generic, geonet::DataRequest& geonet)
{
    geonet.upper_protocol = geonet::UpperProtocol::BTP_B;
    geonet.communication_profile = generic.communication_profile;
    geonet.security_profile = generic.security_profile;
    if (generic.maximum_lifetime) {
        geonet.maximum_lifetime = generic.maximum_lifetime.get();
    }
    geonet.repetition = generic.repetition;
    geonet.traffic_class = generic.traffic_class;
}

geonet::GbcDataRequest Application::request_gbc(const DataRequest& generic)
{
    assert(router_);
    geonet::GbcDataRequest gbc(router_->get_mib());
    initialize_request(generic, gbc);
    gbc.destination = boost::get<geonet::Area>(generic.destination);
    return gbc;
}

geonet::ShbDataRequest Application::request_shb(const DataRequest& generic)
{
    assert(router_);
    geonet::ShbDataRequest shb(router_->get_mib());
    initialize_request(generic, shb);
    return shb;
}
