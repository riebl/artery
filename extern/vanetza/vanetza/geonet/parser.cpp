#include "parser.hpp"
#include "basic_header.hpp"
#include "common_header.hpp"
#include <vanetza/security/exception.hpp>
#include <vanetza/security/secured_message.hpp>

namespace vanetza
{
namespace geonet
{

Parser::Parser(ByteBuffer::const_iterator begin, ByteBuffer::const_iterator end) :
    m_byte_buffer_source(begin, end),
    m_stream(m_byte_buffer_source),
    m_archive(m_stream, boost::archive::no_header),
    m_read_bytes(0)
{
}

Parser::Parser(boost::iterator_range<ByteBuffer::const_iterator> range) :
    Parser(range.begin(), range.end())
{
}

std::size_t Parser::parse_basic(BasicHeader& basic)
{
    std::size_t bytes = 0;
    try {
        deserialize(basic, m_archive);
        bytes = BasicHeader::length_bytes;
    } catch (boost::archive::archive_exception&) {
    }

    m_read_bytes += bytes;
    return bytes;
}

std::size_t Parser::parse_common(CommonHeader& common)
{
    std::size_t bytes = 0;
    try {
        deserialize(common, m_archive);
        bytes = CommonHeader::length_bytes;
    } catch (boost::archive::archive_exception&) {
    }

    m_read_bytes += bytes;
    return bytes;
}

std::size_t Parser::parse_secured(security::SecuredMessageV2& secured)
{
    std::size_t bytes = 0;
    try {
        bytes = deserialize(m_archive, secured);
    } catch (boost::archive::archive_exception&) {
    } catch (security::deserialization_error&) {
    }

    m_read_bytes += bytes;
    return bytes;
}

template<typename EXTENDED>
std::size_t deserialize_extended(InputArchive& archive, HeaderVariant& extended)
{
    EXTENDED header;
    deserialize(header, archive);
    extended = std::move(header);
    return EXTENDED::length_bytes;
}

std::size_t Parser::parse_extended(HeaderVariant& extended, HeaderType ht)
{
    std::size_t bytes = 0;

    try {
        switch (ht) {
            case HeaderType::TSB_SINGLE_HOP:
                bytes = deserialize_extended<ShbHeader>(m_archive, extended);
                break;
            case HeaderType::GEOBROADCAST_CIRCLE:
            case HeaderType::GEOBROADCAST_RECT:
            case HeaderType::GEOBROADCAST_ELIP:
                bytes = deserialize_extended<GeoBroadcastHeader>(m_archive, extended);
                break;
            case HeaderType::BEACON:
                bytes = deserialize_extended<BeaconHeader>(m_archive, extended);
                break;
            case HeaderType::ANY:
            case HeaderType::GEOUNICAST:
            case HeaderType::GEOANYCAST_CIRCLE:
            case HeaderType::GEOANYCAST_RECT:
            case HeaderType::GEOANYCAST_ELIP:
            case HeaderType::TSB_MULTI_HOP:
            case HeaderType::LS_REQUEST:
            case HeaderType::LS_REPLY:
                // unimplemented types
                break;
            default:
                // invalid types
                break;
        }
    } catch (boost::archive::archive_exception&) {
    }

    m_read_bytes += bytes;
    return bytes;
}

std::size_t Parser::parsed_bytes() const
{
    return m_read_bytes;
}

} // namespace geonet
} // namespace vanetza
