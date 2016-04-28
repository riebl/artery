#ifndef DATA_REQUEST_HPP_3OGGPFWF
#define DATA_REQUEST_HPP_3OGGPFWF

#include <vanetza/common/byte_order.hpp>
#include <vanetza/net/access_category.hpp>
#include <vanetza/net/mac_address.hpp>

namespace vanetza
{
namespace access
{

struct DataRequest
{
    DataRequest() : access_category(AccessCategory::BK) {}

    uint16be_t ether_type;
    MacAddress source_addr;
    MacAddress destination_addr;
    AccessCategory access_category;
};

} // namespace access
} // namespace vanetza

#endif /* DATA_REQUEST_HPP_3OGGPFWF */

