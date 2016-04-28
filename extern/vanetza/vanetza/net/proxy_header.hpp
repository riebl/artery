#ifndef PROXY_HEADER_HPP_JWTV3BDT
#define PROXY_HEADER_HPP_JWTV3BDT

#include <vanetza/net/access_category.hpp>
#include <boost/range/iterator_range.hpp>
#include <cstdint>

namespace vanetza
{

#ifndef _MSC_VER
// GCC and Clang support packed attribute
#define PACKED_STRUCT(name, block) struct name { block } __attribute__((__packed__));
#else
// MSVC variant
#define PACKED_STRUCT(name, block) \
__pragma(pack(push, 1)) \
struct name { block }; \
__pragma(pack(pop))
#endif

PACKED_STRUCT(ProxyHeader,
    /** offset in bytes to payload's first byte, counting from packet start */
    uint16_t payload_offset;

    /** length of payload in bytes */
    uint32_t payload_size;

    /** signal power in dBm, fixed point format 8.8 = trunc(double * 256) */
    int16_t signal_power;

    /** access category (transmission only) */
    uint8_t access_category;
)

static_assert(sizeof(ProxyHeader) == 9, "ProxyHeader has invalid size");

double get_signal_power(const ProxyHeader&);
void set_signal_power(ProxyHeader&, double dbm);
AccessCategory get_access_category(const ProxyHeader&);
void set_access_category(ProxyHeader&, AccessCategory);
boost::iterator_range<const uint8_t*> get_payload(const ProxyHeader&, std::size_t);
void set_payload(ProxyHeader&, std::size_t);

} // namespace vanetza

#endif /* PROXY_HEADER_HPP_JWTV3BDT */

