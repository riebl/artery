#ifndef INT_X_HPP_RW3TJBBI
#define INT_X_HPP_RW3TJBBI

#include <vanetza/common/byte_buffer.hpp>
#include <vanetza/security/serialization.hpp>
#include <boost/operators.hpp>
#include <boost/optional.hpp>
#include <cstdint>

namespace vanetza
{
namespace security
{

/// IntX specified in TS 103 097 v1.2.1, section 4.2.1
class IntX : public boost::equality_comparable<IntX>
{
public:
    using integer_type = std::uintmax_t;
    IntX();

    void set(integer_type x);
    integer_type get() const;

    bool operator==(const IntX&) const;

    ByteBuffer encode() const;
    static boost::optional<IntX> decode(const ByteBuffer&);

private:
    integer_type m_value;
};

/**
 * \brief Serializes an IntX into a binary archive
 * \param ar to serialize in
 * \param intx to serialize
 */
void serialize(OutputArchive&, const IntX&);

/**
 * \brief Deserializes an IntX from a binary archive
 * \param ar with a serialized IntX at the beginning
 * \param intx to deserialize
 * \return size of the deserialized IntX
 */
size_t deserialize(InputArchive&, IntX&);

/**
 * \brief Calculates size of an IntX
 * \param intx
 * \return number of octets needed to serialize the IntX
 */
size_t get_size(IntX);

} // namespace security
} // namespace vanetza

#endif /* INT_X_HPP_RW3TJBBI */
