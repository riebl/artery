#include "ethernet_device.hpp"
#include <algorithm>
#include <cstring>
#include <system_error>
#include <linux/if_packet.h>
#include <net/if.h>
#include <sys/ioctl.h>

void initialize(ifreq& request, const char* interface_name)
{
    std::memset(&request, 0, sizeof(ifreq));
    std::strncpy(request.ifr_name, interface_name, IF_NAMESIZE);
    request.ifr_name[IF_NAMESIZE - 1] = '\0';
}

EthernetDevice::EthernetDevice(const char* devname) :
    local_socket_(::socket(AF_LOCAL, SOCK_DGRAM, 0)),
    interface_name_(devname)
{
    if (!local_socket_) {
        throw std::system_error(errno, std::system_category());
    }
}

EthernetDevice::~EthernetDevice()
{
    if (local_socket_ >= 0)
        ::close(local_socket_);
}

EthernetDevice::protocol::endpoint EthernetDevice::endpoint(int family) const
{
    sockaddr_ll socket_address = {0};
    socket_address.sll_family = family;
    socket_address.sll_ifindex = index();
    return protocol::endpoint(&socket_address, sizeof(sockaddr_ll));
}

int EthernetDevice::index() const
{
    ifreq data;
    initialize(data, interface_name_.c_str());
    ::ioctl(local_socket_, SIOCGIFINDEX, &data);
    return data.ifr_ifindex;
}

vanetza::MacAddress EthernetDevice::address() const
{
    ifreq data;
    initialize(data, interface_name_.c_str());
    ::ioctl(local_socket_, SIOCGIFHWADDR, &data);

    vanetza::MacAddress addr;
    std::copy_n(data.ifr_hwaddr.sa_data, addr.octets.size(), addr.octets.data());
    return addr;
}
