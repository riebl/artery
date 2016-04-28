#ifndef MAC_ADDRESS_HPP_FDINBLBS
#define MAC_ADDRESS_HPP_FDINBLBS

#include <boost/operators.hpp>
#include <boost/optional.hpp>
#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <functional>
#include <ostream>
#include <string>

namespace vanetza
{

class MacAddress : public boost::totally_ordered<MacAddress>
{
public:
    static const std::size_t length_bytes = 6;

    MacAddress();
    MacAddress(std::initializer_list<uint8_t> args);

    std::array<uint8_t, length_bytes> octets;
};

extern const MacAddress cBroadcastMacAddress;
bool operator==(const MacAddress& lhs, const MacAddress& rhs);
bool operator<(const MacAddress& lhs, const MacAddress& rhs);
std::ostream& operator<<(std::ostream& os, const MacAddress&);

/**
 * Try to parse MAC address from string
 * \param str source string with "XX:XX:XX:XX:XX:XX" format
 * \param addr pass parsed address by reference
 * \return true if successfully parsed
 */
bool parse_mac_address(const std::string& str, MacAddress& addr);

/**
 * Try to parse MAC address from string
 * \param str source string with "XX:XX:XX:XX:XX:XX" format
 * \return parsed address if successful
 */
boost::optional<MacAddress> parse_mac_address(const std::string& str);

/**
 * Derive a MAC address from an arbitrary integral value.
 * \param value used to derive MAC address, it's size does not matter
 * \return New MAC address
 */
template<typename T>
MacAddress create_mac_address(T value)
{
    MacAddress mac;
    const std::size_t octets = std::min(mac.octets.size(), sizeof(T));
    for (std::size_t i = 0; i < octets; ++i) {
        mac.octets[i] = value >> (8 * (octets - i - 1)) & 0xff;
    }
    return mac;
}

} // namespace vanetza

// specialization
namespace std {

template<>
struct hash<vanetza::MacAddress>
{
    size_t operator()(const vanetza::MacAddress& addr) const
    {
        size_t tmp = 0;
        for (auto octet : addr.octets) {
            tmp ^= hash<decltype(octet)>()(octet);
        }
        return tmp;
    }
};

} // namespace std

#endif /* MAC_ADDRESS_HPP_FDINBLBS */

