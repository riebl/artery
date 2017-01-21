#include "hello_application.hpp"
#include <chrono>
#include <functional>
#include <iostream>

using namespace vanetza;

HelloApplication::HelloApplication(boost::asio::steady_timer& timer) : timer_(timer)
{
    schedule_timer();
}

HelloApplication::PortType HelloApplication::port()
{
    return host_cast<uint16_t>(42);
}

void HelloApplication::indicate(const DataIndication& indication, UpPacketPtr packet)
{
    std::cout << "HelloApplication received a packet\n";
}

void HelloApplication::schedule_timer()
{
    timer_.expires_from_now(std::chrono::seconds(2));
    timer_.async_wait(std::bind(&HelloApplication::on_timer, this, std::placeholders::_1));
}

void HelloApplication::on_timer(const boost::system::error_code& ec)
{
    if (ec != boost::asio::error::operation_aborted) {
        DownPacketPtr packet { new DownPacket() };
        packet->layer(OsiLayer::Application) = ByteBuffer { 0xC0, 0xFF, 0xEE };
        DataRequest request;
        request.transport_type = geonet::TransportType::SHB;
        request.communication_profile = geonet::CommunicationProfile::ITS_G5;
        auto confirm = Application::request(request, std::move(packet));
        if (!confirm.accepted()) {
            std::cerr << "HelloApplication data request failed\n";
        }

        schedule_timer();
    }
}
