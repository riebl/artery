#ifndef LIFETIME_HPP_XYTSNW3J
#define LIFETIME_HPP_XYTSNW3J

#include <vanetza/common/bit_number.hpp>
#include <vanetza/geonet/serialization.hpp>
#include <vanetza/units/time.hpp>
#include <boost/operators.hpp>
#include <cstdint>

namespace vanetza
{
namespace geonet
{

class Lifetime : public boost::totally_ordered<Lifetime>
{
public:
    static const uint8_t multiplier_mask = 0xFC;
    static const uint8_t base_mask = 0x03;

    enum class Base {
        _50_MS = 0,
        _1_S = 1,
        _10_S = 2,
        _100_S = 3
    };

    Lifetime();
    Lifetime(Base base, BitNumber<uint8_t, 6> multiplier);
    void set(Base base, BitNumber<uint8_t, 6> multiplier);
    uint8_t raw() const { return m_lifetime; }
    void raw(uint8_t raw) { m_lifetime = raw; }

    bool operator<(const Lifetime&) const;
    bool operator==(const Lifetime&) const;

    /**
     * Decodes stored lifetime
     * \return lifetime as duration quantity
     */
    units::Duration decode() const;

    /**
     * Encode duration in lifetime object
     * \note Precision loss might occur
     * \param duration Lifetime duration
     */
    void encode(units::Duration);

private:
    uint8_t m_lifetime;
};

void serialize(const Lifetime&, OutputArchive&);
void deserialize(Lifetime&, InputArchive&);

} // namespace geonet
} // namespace vanetza

#endif /* LIFETIME_HPP_XYTSNW3J */

