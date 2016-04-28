#include "proxy_handler.hpp"
#include <boost/asio/error.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>
#include <cmath>
#include <iostream>

using namespace boost;

ProxyHandler::ProxyHandler(udp_socket& udp, cohda_socket& cohda) :
    m_udp(udp), m_cohda(cohda)
{
    m_mk2_tx.ChannelNumber = 180;
    m_mk2_tx.MCS = MK2MCS_R12QPSK;
    m_mk2_tx.Priority = MK2_PRIO_0;
    m_mk2_tx.Service = MK2_QOS_ACK;
    m_mk2_tx.Expiry = 0;
    m_mk2_tx.TxAntenna = MK2_TXANT_DEFAULT;
    m_mk2_tx.TxPower.PowerSetting = MK2TPC_MANUAL;
    m_mk2_tx.TxPower.ManualPower = 40; // 40 * 0.5 dBm = 20 dBm
}

void ProxyHandler::channel_number(tMK2Channel channel)
{
    m_mk2_tx.ChannelNumber = channel;
}

void ProxyHandler::mcs(tMK2MCS mcs)
{
    m_mk2_tx.MCS = mcs;
}

void ProxyHandler::start_transfer()
{
    m_udp.async_receive_from(asio::buffer(m_udp2ota_buffer), m_udp_receiver,
            bind(&ProxyHandler::handle_udp_receive_from, this,
                asio::placeholders::error,
                asio::placeholders::bytes_transferred));
}

void ProxyHandler::next_udp_transfer()
{
    m_udp.async_receive(asio::buffer(m_udp2ota_buffer),
            bind(&ProxyHandler::handle_udp_receive, this,
                asio::placeholders::error,
                asio::placeholders::bytes_transferred));
}

void ProxyHandler::handle_udp_receive_from(system::error_code ec, size_t bytes_received)
{
    if (!ec) {
        next_ota_transfer();
    }

    handle_udp_receive(ec, bytes_received);
}

void ProxyHandler::handle_udp_receive(system::error_code ec, size_t bytes_received)
{
    if (!ec && bytes_received >= sizeof(vanetza::ProxyHeader)) {
        vanetza::ProxyHeader* header =
            asio::buffer_cast<vanetza::ProxyHeader*>(asio::buffer(m_udp2ota_buffer));
        send_ota(*header, bytes_received);
    } else {
        next_udp_transfer();
    }
}

void ProxyHandler::handle_udp_send(system::error_code, size_t /* bytes_transferred */)
{
    next_ota_transfer();
}

void ProxyHandler::next_ota_transfer()
{
    m_cohda.async_receive(asio::buffer(m_ota2udp_buffer),
            bind(&ProxyHandler::handle_ota_receive, this,
                asio::placeholders::error,
                asio::placeholders::bytes_transferred));
}

void ProxyHandler::handle_ota_receive(system::error_code ec, size_t bytes_received)
{
    if (!ec && bytes_received >= sizeof(tMK2RxDescriptor)) {
        tMK2RxDescriptor* mk2_rx = reinterpret_cast<tMK2RxDescriptor*>(&m_ota2udp_buffer[0]);
        const double signalpower_dbm = mk2_rx->RxPowerA / 2.0;
        const void* payload = &m_ota2udp_buffer[sizeof(tMK2RxDescriptor)];
        const unsigned fcs_length = 4; // FCS checksum bytes
        // TODO: Don't merely skip FCS bytes, but check for correctness
        send_udp(signalpower_dbm, payload, bytes_received - sizeof(tMK2RxDescriptor) - fcs_length);
    } else {
        next_ota_transfer();
    }
}

void ProxyHandler::handle_ota_send(system::error_code, size_t /* bytes_transferred */)
{
    next_udp_transfer();
}

void ProxyHandler::send_ota(const vanetza::ProxyHeader& header, std::size_t length)
{
    auto payload = get_payload(header, length);
    double power = get_signal_power(header);
    m_mk2_tx.TxPower.ManualPower = std::round(power * 2);
    // I suppose MK2 priorities are derived from 802.1d
    switch (get_access_category(header)) {
        case vanetza::AccessCategory::BK:
            m_mk2_tx.Priority = MK2_PRIO_1;
            break;
        case vanetza::AccessCategory::BE:
            m_mk2_tx.Priority = MK2_PRIO_0;
            break;
        case vanetza::AccessCategory::VI:
            m_mk2_tx.Priority = MK2_PRIO_5;
            break;
        case vanetza::AccessCategory::VO:
            m_mk2_tx.Priority = MK2_PRIO_6;
            break;
        default:
            assert(false && "Unknown access category");
            break;
    };

    array<asio::const_buffer, 2> buffers = {
        asio::buffer(&m_mk2_tx, sizeof(m_mk2_tx)),
        asio::buffer(begin(payload), size(payload))
    };
    m_cohda.async_send(buffers,
            bind(&ProxyHandler::handle_ota_send, this,
                asio::placeholders::error,
                asio::placeholders::bytes_transferred));

    std::cout << "-> " << size(payload) << " bytes / "
        << power << " dBm / "
        << get_access_category(header) << "\n";
}

void ProxyHandler::send_udp(double signalpower_dbm, const void* buf, std::size_t length)
{
    set_signal_power(m_proxy_header, signalpower_dbm);
    set_payload(m_proxy_header, length);

    array<asio::const_buffer, 2> buffers = {
        asio::buffer(&m_proxy_header, sizeof(m_proxy_header)),
        asio::buffer(buf, length)
    };
    m_udp.async_send_to(buffers, m_udp_receiver,
            bind(&ProxyHandler::handle_udp_send, this,
                asio::placeholders::error,
                asio::placeholders::bytes_transferred));

    std::cout << "<- " << length << " bytes / "
        << signalpower_dbm << " dBm\n";
}

