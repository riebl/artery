#include <vanetza/common/byte_order.hpp>
#include <vanetza/net/ethernet_header.hpp>
#include <vanetza/net/mac_address.hpp>
#include <vanetza/net/buffer_packet.hpp>
#include <vanetza/net/proxy_header.hpp>
#include <boost/array.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/lexical_cast.hpp>
#include <exception>
#include <iostream>

using namespace boost;
using namespace vanetza;

/**
 * This application sends periodic messages (proxy header and fake data)
 * to a proxy implementation listening on an UDP socket.
 */

[[noreturn]] void terminate_handler() noexcept
{
    if (auto ce = std::current_exception()) {
        try {
            std::rethrow_exception(ce);
        } catch (const std::exception& e) {
            std::cerr << "Program exit because of " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Program exit because unknown error" << std::endl;
        }
    }
    std::exit(1);
}

int main(int argc, char** argv)
{
    std::set_terminate(&terminate_handler);
    asio::ip::address_v4 dst_addr;
    unsigned short dst_port = 8041; // Boost.Asio expects port in host byte order

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " ip_addr [port_number]\n";
        return 1;
    } else {
        dst_addr = asio::ip::address_v4::from_string(argv[1]);
        if (argc > 2) {
            dst_port = boost::lexical_cast<unsigned short>(argv[2]);
        }
    }

    asio::io_service io_service;
    asio::ip::udp::socket socket(io_service, asio::ip::udp::v4());
    asio::ip::udp::endpoint endpoint(dst_addr, dst_port);

    MacAddress dst_mac { 0xde, 0xad, 8, 8, 8, 1 };
    MacAddress src_mac { 0xde, 0xad, 8, 8, 8, 2 };
    uint16be_t eth_proto = host_cast<uint16_t>(0x0707u);

    BufferPacket packet;
    packet[OsiLayer::Link] = create_ethernet_header(dst_mac, src_mac, eth_proto);
    packet[OsiLayer::Network].assign(10, 0xab);

    ProxyHeader proxy;
    set_signal_power(proxy, 20.0);
    set_payload(proxy, packet.size());

    while (true) {
        array<asio::const_buffer, 3> buffers {{
            asio::buffer(&proxy, sizeof(proxy)),
            asio::buffer(packet[OsiLayer::Link]),
            asio::buffer(packet[OsiLayer::Network])
        }};
        socket.send_to(buffers, endpoint);

        sleep(1);
    }

    return 0;
}

