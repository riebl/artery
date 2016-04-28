#include <vanetza/geonet/basic_header.hpp>
#include <vanetza/geonet/common_header.hpp>
#include <vanetza/geonet/header_variant.hpp>
#include <vanetza/geonet/pdu.hpp>
#include <vanetza/security/secured_message.hpp>

namespace vanetza
{
namespace geonet
{

void serialize(const ConstAccessiblePdu& pdu, OutputArchive& ar)
{
    serialize(pdu.basic(), ar);
    if (pdu.secured()) {
        security::serialize(ar, *pdu.secured());
    } else {
        geonet::serialize(pdu.common(), ar);
        boost::serialize(pdu.extended_variant(), ar);
    }
}

std::size_t get_length(const ConstAccessiblePdu& pdu)
{
    std::size_t length = BasicHeader::length_bytes;
    if (pdu.secured()) {
        length += get_size(*pdu.secured());
    } else {
        length += CommonHeader::length_bytes;
        length += get_length(pdu.extended_variant());
    }
    return length;
}

} // namespace geonet
} // namespace vanetza
