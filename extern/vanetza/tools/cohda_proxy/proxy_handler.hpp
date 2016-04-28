#ifndef PROXY_HANDLER_HPP_GNVISFKK
#define PROXY_HANDLER_HPP_GNVISFKK

#include <vanetza/asio/cohda.hpp>
#include <vanetza/net/proxy_header.hpp>
#include <boost/array.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/ip/udp.hpp>
#include <cstdint>
#include <mk2mac-api-types.h>

class ProxyHandler
{
    public:
    typedef boost::asio::ip::udp::socket udp_socket;
    typedef vanetza::asio::cohda::socket cohda_socket;

    ProxyHandler(udp_socket&, cohda_socket&);
    void start_transfer();
    void channel_number(tMK2Channel);
    void mcs(tMK2MCS);

    private:
    void next_ota_transfer();
    void next_udp_transfer();
    void handle_udp_receive_from(boost::system::error_code, size_t bytes_received);
    void handle_udp_receive(boost::system::error_code, size_t bytes_received);
    void handle_udp_send(boost::system::error_code, size_t bytes_transferred);
    void handle_ota_send(boost::system::error_code, size_t bytes_transferred);
    void handle_ota_receive(boost::system::error_code, size_t bytes_received);
    void send_ota(const vanetza::ProxyHeader&, std::size_t length);
    void send_udp(double signalpower_dbm, const void* buffer, std::size_t length);

    private:
    udp_socket& m_udp;
    cohda_socket& m_cohda;
    boost::asio::ip::udp::endpoint m_udp_receiver;
    boost::array<uint8_t, 2048> m_udp2ota_buffer;
    boost::array<uint8_t, 2048> m_ota2udp_buffer;
    tMK2TxDescriptor m_mk2_tx;
    vanetza::ProxyHeader m_proxy_header;
};

#endif /* PROXY_HANDLER_HPP_GNVISFKK */

