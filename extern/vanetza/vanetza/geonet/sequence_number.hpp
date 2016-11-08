#ifndef SEQUENCE_NUMBER_HPP_HEO4A3XC
#define SEQUENCE_NUMBER_HPP_HEO4A3XC

#include <vanetza/geonet/serialization.hpp>
#include <boost/operators.hpp>
#include <cstdint>
#include <limits>

namespace vanetza
{
namespace geonet
{

class SequenceNumber :
    public boost::totally_ordered<SequenceNumber>,
    public boost::additive<SequenceNumber>
{
public:
    using value_type = uint16_t;
    static constexpr value_type max = std::numeric_limits<value_type>::max();

    SequenceNumber() : m_number(0) {}
    explicit SequenceNumber(value_type number) : m_number(number) {}
    explicit operator value_type() const { return m_number; }
    bool operator<(SequenceNumber other) const;
    bool operator==(SequenceNumber other) const { return m_number == other.m_number; }
    void operator+=(SequenceNumber other) { m_number += other.m_number; }
    void operator-=(SequenceNumber other) { m_number -= other.m_number; }
    SequenceNumber operator++(int);

private:
    value_type m_number;
};

void serialize(const SequenceNumber&, OutputArchive&);
void deserialize(SequenceNumber&, InputArchive&);

} // namespace geonet
} // namespace vanetza

#endif /* SEQUENCE_NUMBER_HPP_HEO4A3XC */

