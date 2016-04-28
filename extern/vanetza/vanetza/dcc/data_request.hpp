#ifndef DATA_REQUEST_HPP_A5DWTTJN
#define DATA_REQUEST_HPP_A5DWTTJN

#include <vanetza/common/byte_order.hpp>
#include <vanetza/common/clock.hpp>
#include <vanetza/dcc/profile.hpp>
#include <vanetza/net/mac_address.hpp>

namespace vanetza
{
namespace dcc
{

struct DataRequest
{
    DataRequest() : dcc_profile(Profile::DP0) {}

    uint16be_t ether_type;
    MacAddress source;
    MacAddress destination;
    Profile dcc_profile;
    Clock::duration lifetime;
};

} // namespace dcc
} // namespace vanetza

#endif /* DATA_REQUEST_HPP_A5DWTTJN */

