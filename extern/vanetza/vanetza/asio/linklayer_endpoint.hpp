#ifndef LINKLAYER_ENDPOINT_HPP_DMNJI8U6
#define LINKLAYER_ENDPOINT_HPP_DMNJI8U6

#include <boost/asio/detail/socket_ops.hpp>
#include <boost/asio/detail/socket_types.hpp>
#include <netpacket/packet.h>

namespace vanetza
{
namespace asio
{

template<typename Protocol>
class linklayer_endpoint
{
    public:
    typedef boost::asio::detail::socket_addr_type sockaddr_type;
    typedef sockaddr_ll sockaddr_ll_type;

    linklayer_endpoint() : m_data() {
        Protocol protocol;
        m_data.link.sll_family = protocol.family();
        m_data.link.sll_protocol = boost::asio::detail::socket_ops::host_to_network_short(protocol.protocol());
        m_data.link.sll_ifindex = protocol.interface_index();
    }

    linklayer_endpoint(const Protocol& protocol) : m_data() {
        m_data.link.sll_family = protocol.family();
        m_data.link.sll_protocol = boost::asio::detail::socket_ops::host_to_network_short(protocol.protocol());
        m_data.link.sll_ifindex = protocol.interface_index();
    }

    const sockaddr_type* data() const
    {
        return &m_data.base;
    }

    std::size_t size() const
    {
        return sizeof(sockaddr_ll_type);
    }

    Protocol protocol() const { return Protocol(); }

    private:
    union data_union {
        sockaddr_type base;
        sockaddr_ll_type link;
    } m_data;
};

} // namespace asio
} // namespace vanetza

#endif /* LINKLAYER_ENDPOINT_HPP_DMNJI8U6 */

