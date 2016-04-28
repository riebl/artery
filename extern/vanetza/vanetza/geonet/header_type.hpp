#ifndef HEADER_TYPE_HPP_U5FGWR9N
#define HEADER_TYPE_HPP_U5FGWR9N

#include <cstdint>

namespace vanetza
{
namespace geonet
{

// forward declaration
struct Area;

namespace nibble
{
    namespace high
    {
        const uint8_t any = 0x00;
        const uint8_t beacon = 0x10;
        const uint8_t unicast = 0x20;
        const uint8_t anycast = 0x30;
        const uint8_t broadcast = 0x40;
        const uint8_t tsb = 0x50;
        const uint8_t ls = 0x60;
    } // namespace high
    namespace low
    {
        const uint8_t circle = 0x00;
        const uint8_t rect = 0x01;
        const uint8_t elip = 0x02;
        const uint8_t single_hop = 0x00;
        const uint8_t multi_hop = 0x01;
    } // namespace low
} // namespace nibble

enum class HeaderType : uint8_t
{
    // high nibble: type, low nibble: sub-type
    ANY = nibble::high::any,
    BEACON = nibble::high::beacon,
    GEOUNICAST = nibble::high::unicast,
    GEOANYCAST_CIRCLE = nibble::high::anycast | nibble::low::circle,
    GEOANYCAST_RECT = nibble::high::anycast | nibble::low::rect,
    GEOANYCAST_ELIP = nibble::high::anycast | nibble::low::elip,
    GEOBROADCAST_CIRCLE = nibble::high::broadcast | nibble::low::circle,
    GEOBROADCAST_RECT = nibble::high::broadcast | nibble::low::rect,
    GEOBROADCAST_ELIP = nibble::high::broadcast | nibble::low::elip,
    TSB_SINGLE_HOP = nibble::high::tsb | nibble::low::single_hop,
    TSB_MULTI_HOP = nibble::high::tsb | nibble::low::multi_hop,
    LS_REQUEST = nibble::high::ls | 0x00,
    LS_REPLY = nibble::high::ls | 0x01
};

inline uint8_t high_nibble(HeaderType ht) { return static_cast<uint8_t>(ht) & 0xf0; }
inline uint8_t low_nibble(HeaderType ht) { return static_cast<uint8_t>(ht) & 0x0f; }

HeaderType gbc_header_type(const Area&);
HeaderType gac_header_type(const Area&);

} // namespace geonet
} // namespace vanetza

#endif /* HEADER_TYPE_HPP_U5FGWR9N */

