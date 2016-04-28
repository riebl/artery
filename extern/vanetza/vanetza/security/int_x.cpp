#include <vanetza/security/int_x.hpp>
#include <vanetza/security/length_coding.hpp>

namespace vanetza
{
namespace security
{

IntX::IntX() : m_value(0) {}

void IntX::set(integer_type x)
{
    m_value = x;
}

IntX::integer_type IntX::get() const
{
    return m_value;
}

bool IntX::operator==(const IntX& other) const
{
    return this->m_value == other.m_value;
}

ByteBuffer IntX::encode() const
{
    return encode_length(m_value);
}

boost::optional<IntX> IntX::decode(const ByteBuffer& buffer)
{
    boost::optional<IntX> result;
    auto decoded_tuple = decode_length(buffer);
    if (decoded_tuple) {
        IntX tmp;
        tmp.set(std::get<1>(*decoded_tuple));
        result = tmp;
    }
    return result;
}

size_t get_size(IntX intx)
{
    return length_coding_size(intx.get());
}

void serialize(OutputArchive& ar, const IntX& intx)
{
    serialize_length(ar, intx.get());
}

size_t deserialize(InputArchive& ar, IntX& intx)
{
    const auto size = deserialize_length(ar);
    intx.set(size);
    return get_size(intx);
}

} // namespace security
} // namespace vanetza
