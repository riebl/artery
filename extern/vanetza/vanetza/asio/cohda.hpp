#ifndef COHDA_HPP_CCA3HKJO
#define COHDA_HPP_CCA3HKJO

#include <vanetza/asio/linklayer_endpoint.hpp>
#include <boost/asio/basic_raw_socket.hpp>
#include <linux/if_ether.h>
#include <net/if.h>

namespace vanetza
{
namespace asio
{

class cohda
{
    public:
    typedef linklayer_endpoint<cohda> endpoint;
    typedef boost::asio::basic_raw_socket<cohda> socket;

    static cohda wave_raw()
    {
        return cohda("wave-raw", ETH_P_ALL);
    }

    explicit cohda() : m_protocol(ETH_P_ALL), m_ifidx(0) {}

    int type() const { return SOCK_RAW; }
    int family() const { return PF_PACKET; }
    int protocol() const { return m_protocol; }
    int interface_index() const { return m_ifidx; }

    private:
    explicit cohda(const char* if_name, int proto) :
        m_protocol(proto), m_ifidx(if_nametoindex(if_name)) {}

    int m_protocol; /*< ethernet protocol number */
    int m_ifidx; /*< network interface card number */
};

} // namespace asio
} // namespace vanetza

#endif /* COHDA_HPP_CCA3HKJO */

