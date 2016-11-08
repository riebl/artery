#ifndef GEONET_ADDRESS_HPP_MB8J1IVQ
#define GEONET_ADDRESS_HPP_MB8J1IVQ

#include <vanetza/common/bit_number.hpp>
#include <vanetza/net/mac_address.hpp>
#include <vanetza/geonet/serialization.hpp>
#include <vanetza/geonet/station_type.hpp>
#include <functional>
#include <type_traits>

namespace vanetza
{
namespace geonet
{

class Address
{
public:
    static constexpr std::size_t length_bytes = 8;

    Address();
    explicit Address(const MacAddress&);
    bool is_manually_configured() const { return m_manually_configured; }
    void is_manually_configured(bool flag) { m_manually_configured = flag; }
    StationType station_type() const { return m_station_type; }
    void station_type(StationType type) { m_station_type = type; }
    BitNumber<unsigned, 10> country_code() const { return m_country_code; }
    void country_code(BitNumber<unsigned, 10> country) { m_country_code = country; }
    const MacAddress& mid() const { return m_mid; }
    void mid(const MacAddress& mid) { m_mid = mid; }

    bool operator==(const Address& other) const;
    bool operator!=(const Address& other) const;

private:
    bool m_manually_configured; // 1 bit
    StationType m_station_type; // 5 bit
    BitNumber<unsigned, 10> m_country_code; // 10 bit
    MacAddress m_mid; // 48 bit
};

void serialize(const Address&, OutputArchive&);
void serialize(const MacAddress&, OutputArchive&);
void deserialize(Address&, InputArchive&);
void deserialize(MacAddress&, InputArchive&);

} // namespace geonet
} // namespace vanetza


namespace std {

template<>
struct hash<vanetza::geonet::Address>
{
    size_t operator()(const vanetza::geonet::Address& addr) const;
};

} // namespace std

#endif /* GEONET_ADDRESS_HPP_MB8J1IVQ */

