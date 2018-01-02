#ifndef PARSER_HPP_IBDRMPKB
#define PARSER_HPP_IBDRMPKB

#include <vanetza/common/byte_buffer.hpp>
#include <vanetza/common/byte_buffer_source.hpp>
#include <vanetza/geonet/header_type.hpp>
#include <vanetza/geonet/header_variant.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/range/iterator_range.hpp>
#include <cstdint>

namespace vanetza
{

// forward declaration
namespace security { struct SecuredMessageV2; }

namespace geonet
{

// forward declarations
struct BasicHeader;
struct CommonHeader;

class Parser
{
public:
    Parser(ByteBuffer::const_iterator begin, ByteBuffer::const_iterator end);
    Parser(boost::iterator_range<ByteBuffer::const_iterator> range);

    std::size_t parse_basic(BasicHeader&);
    std::size_t parse_common(CommonHeader&);
    std::size_t parse_secured(security::SecuredMessageV2&);
    std::size_t parse_extended(HeaderVariant&, HeaderType);
    std::size_t parsed_bytes() const;

private:
    byte_buffer_source m_byte_buffer_source;
    boost::iostreams::stream_buffer<byte_buffer_source> m_stream;
    InputArchive m_archive;
    std::size_t m_read_bytes;
};

} // namespace geonet
} // namespace vanetza

#endif /* PARSER_HPP_IBDRMPKB */

