#ifndef HEADER_CONVERSION_HPP_SQVEUMFE
#define HEADER_CONVERSION_HPP_SQVEUMFE

#include <vanetza/btp/header.hpp>
#include <vanetza/common/byte_buffer_convertible.hpp>
#include <vanetza/common/serialization_buffer.hpp>

namespace vanetza
{
namespace convertible
{

template<>
struct byte_buffer_impl<btp::HeaderB&> : public byte_buffer
{
    byte_buffer_impl(const btp::HeaderB& header) : m_header(header) {}
    void convert(ByteBuffer& buffer) const override
    {
        serialize_into_buffer(m_header, buffer);
    }
    std::size_t size() const override { return btp::HeaderB::length_bytes; }

    const btp::HeaderB m_header;
};

} // namespace convertible
} // namespace vanetza

#endif /* HEADER_CONVERSION_HPP_SQVEUMFE */

