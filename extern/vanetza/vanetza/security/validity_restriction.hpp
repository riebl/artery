#ifndef VALIDITY_RESTRICTION_HPP_LMCUHYLJ
#define VALIDITY_RESTRICTION_HPP_LMCUHYLJ

#include <vanetza/common/bit_number.hpp>
#include <vanetza/security/region.hpp>
#include <vanetza/security/serialization.hpp>
#include <boost/variant/variant.hpp>

namespace vanetza
{
namespace security
{

using Time32 = uint32_t;
using Time64 = uint64_t;

class Duration
{
public:
    enum class Units
    {
        Seconds = 0x0,
        Minutes = 0x1,
        Hours = 0x2,
        Sixty_Hour_Blocks = 0x3,
        Years = 0x4
    };

    Duration();
    Duration(BitNumber<uint16_t, 13> value, Units unit);
    Duration(uint16_t raw);

    uint16_t raw() const
    {
        return m_raw;
    }

private:
    uint16_t m_raw;
};

size_t get_size(const Duration&);

enum class ValidityRestrictionType : uint8_t
{
    Time_End = 0,                   // end_validity
    Time_Start_And_End = 1,         // start_and_end_valididty
    Time_Start_And_Duration = 2,    // start_and_duration_validity
    Region = 3                      // GeographicRegion
};

struct StartAndEndValidity
{
    Time32 start_validity;
    Time32 end_validity;
};

struct StartAndDurationValidity
{
    Time32 start_validity;
    Duration duration;
};

using EndValidity = Time32;

typedef boost::variant<EndValidity, StartAndEndValidity, StartAndDurationValidity, GeographicRegion> ValidityRestriction;

/**
 * Determines ValidityRestrictionType to a given ValidityRestriction
 * \param ValidityRestriction
 */
ValidityRestrictionType get_type(const ValidityRestriction&);

/**
 * Calculates size of an object
 * \param Object
 * \return size_t containing the number of octets needed to serialize the object
 */
size_t get_size(const StartAndEndValidity&);
size_t get_size(const StartAndDurationValidity&);
size_t get_size(const ValidityRestriction&);
size_t get_size(const Time32&);
size_t get_size(const Time64&);
size_t get_size(const Duration&);

/**
 * Deserializes a ValidityRestriction list from a binary archive
 * \param archive with a serialized ValidityRestriction list at the beginning
 * \param ValidityRestriction list to deserialize
 * \return size of the ValidityRestriction list
 */
size_t deserialize(InputArchive&, ValidityRestriction&);

/**
 * Serializes a ValidityRestriction list into a binary archive
 * \param archive to serialize in
 * \param ValidityRestriction list to serialize
 */
void serialize(OutputArchive&, const ValidityRestriction&);

} // namespace security
} // namespace vanetza

#endif /* VALIDITY_RESTRICTION_HPP_LMCUHYLJ */

