#include "proxy_header.hpp"
#include <vanetza/common/byte_order.hpp>
#include <cassert>

namespace vanetza
{

double get_signal_power(const ProxyHeader& header)
{
    double power = ntoh(header.signal_power);
    power /= 256.0;
    return power;
}

void set_signal_power(ProxyHeader& header, double dbm)
{
    decltype(ProxyHeader::signal_power) power = dbm * 256;
    header.signal_power = hton(power);
}

AccessCategory get_access_category(const ProxyHeader& header)
{
    return static_cast<AccessCategory>(header.access_category & 0x03);
}

void set_access_category(ProxyHeader& header, AccessCategory ac)
{
    header.access_category = static_cast<uint8_t>(ac);
}

boost::iterator_range<const uint8_t*> get_payload(const ProxyHeader& header, std::size_t size)
{
    const auto payload_offset = ntoh(header.payload_offset);
    const auto payload_size = ntoh(header.payload_size);

    assert(size >= payload_offset + payload_size);
    assert(payload_offset >= sizeof(ProxyHeader));

    const uint8_t* begin = reinterpret_cast<const uint8_t*>(&header);
    const uint8_t* end = begin;

    if (size >= payload_offset + payload_size) {
        begin += payload_offset;
        end = begin + payload_size;
    }

    return boost::make_iterator_range(begin, end);
}

void set_payload(ProxyHeader& header, std::size_t size)
{
    header.payload_offset = hton<decltype(header.payload_offset)>(sizeof(ProxyHeader));
    header.payload_size = hton<decltype(header.payload_size)>(size);
}

} // namespace vanetza

