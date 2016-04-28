#ifndef BASIC_HEADER_HPP_8QS7WLG3
#define BASIC_HEADER_HPP_8QS7WLG3

#include <vanetza/common/bit_number.hpp>
#include <vanetza/geonet/lifetime.hpp>
#include <vanetza/geonet/mib.hpp>
#include <vanetza/geonet/serialization.hpp>

namespace vanetza
{
namespace geonet
{

struct DataRequest;
struct ShbDataRequest;

/// NextHeaderBasic specified in ETSI EN 302 636-4-1 v1.2.1, section 8.6.3
enum class NextHeaderBasic : uint8_t
{
    ANY = 0,
    COMMON = 1,
    SECURED = 2,
};

/// BasicHeader specified in ETSI EN 302 636-4-1 v1.2.1, section 8.6
struct BasicHeader
{
    BasicHeader();
    BasicHeader(const MIB&);
    BasicHeader(const DataRequest&, const MIB&);
    BasicHeader(const ShbDataRequest&, const MIB&);

    static const std::size_t length_bytes = 3 + sizeof(Lifetime);

    BitNumber<unsigned, 4> version;
    NextHeaderBasic next_header; // 4 bit
    uint8_t reserved;
    Lifetime lifetime;
    uint8_t hop_limit;
};

/**
 * \brief Serializes a BasicHeader into a binary archive
 * \param basic to serialize
 * \param ar to serialize in
 */
void serialize(const BasicHeader&, OutputArchive&);

/**
 * \brief Deserializes a BasicHeader from a binary archive
 * \param basic to deserialize
 * \param ar with a serialized BasicHeader at the beginning
 */
void deserialize(BasicHeader&, InputArchive&);

} // namespace geonet
} // namespace vanetza

#endif /* BASIC_HEADER_HPP_8QS7WLG3 */
