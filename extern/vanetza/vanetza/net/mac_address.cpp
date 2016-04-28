#include "mac_address.hpp"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/io/ios_state.hpp>
#include <algorithm>
#include <cassert>
#include <cctype>
#include <iomanip>
#include <string>
#include <vector>

namespace vanetza
{

const MacAddress cBroadcastMacAddress = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
const std::size_t MacAddress::length_bytes;

MacAddress::MacAddress()
{
    std::fill_n(octets.begin(), octets.size(), 0x00);
}

MacAddress::MacAddress(std::initializer_list<uint8_t> args)
{
    assert(args.size() == octets.size());
    std::copy_n(args.begin(), std::min(args.size(), octets.size()), octets.begin());
}

bool operator==(const MacAddress& lhs, const MacAddress& rhs)
{
    return (lhs.octets == rhs.octets);
}

bool operator<(const MacAddress& lhs, const MacAddress& rhs)
{
    for (std::size_t i = 0; i < MacAddress::length_bytes; ++i) {
        if (lhs.octets[i] < rhs.octets[i]) {
            return true;
        }
    }
    return false;
}

bool parse_mac_address(const std::string& str, MacAddress& addr)
{
    bool parsed = false;
    std::vector<std::string> octets;
    boost::algorithm::split(octets, str, boost::algorithm::is_any_of(":"));

    // lambda returning true if string consists of two hex digits
    auto octet_checker = [](const std::string& str) {
        return str.size() == 2 && std::isxdigit(str[0]) && std::isxdigit(str[1]);
    };

    if (octets.size() == addr.octets.size() && std::all_of(octets.begin(), octets.end(), octet_checker)) {
        std::transform(octets.begin(), octets.end(), addr.octets.begin(),
                [](const std::string& octet) {
                    return std::strtol(octet.c_str(), nullptr, 16);
                });
        parsed = true;
    }

    return parsed;
}

boost::optional<MacAddress> parse_mac_address(const std::string& str)
{
    boost::optional<MacAddress> addr_result;
    MacAddress addr_tmp;
    if (parse_mac_address(str, addr_tmp)) {
        addr_result.reset(addr_tmp);
    }
    return addr_result;
}

std::ostream& operator<<(std::ostream& os, const MacAddress& addr)
{
    boost::io::ios_all_saver ifs(os);
    os << std::hex << std::setfill('0');
    os << std::setw(2) << unsigned(addr.octets[0]);
    for (unsigned i = 1; i < addr.octets.size(); ++i) {
        os << ":" << std::setw(2) << unsigned(addr.octets[i]);
    }
    return os;
}

} // namespace vanetza

