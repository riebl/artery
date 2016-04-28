#include "proxy_handler.hpp"
#include <vanetza/asio/cohda.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/udp.hpp>
#include <mk2mac-api-types.h>
#include <iostream>

using namespace boost;

int main(int /* argc */, char** /* argv */)
{
    const tMK2Channel pwlan_channel = 180;
    const tMK2MCS pwlan_mcs = MK2MCS_R12QPSK;
    const unsigned udp_port = 8041;

    try {
        asio::io_service io_service;
        asio::ip::udp::endpoint udp_endpoint = { asio::ip::udp::v4(), udp_port };
        asio::ip::udp::socket udp_socket = { io_service, udp_endpoint };
        vanetza::asio::cohda::endpoint cohda_endpoint = { vanetza::asio::cohda::wave_raw() };
        vanetza::asio::cohda::socket cohda_socket = { io_service, cohda_endpoint };
        ProxyHandler handler(udp_socket, cohda_socket);
        handler.channel_number(pwlan_channel);
        handler.mcs(pwlan_mcs);
        handler.start_transfer();
        io_service.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}

