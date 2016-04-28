#include <vanetza/geonet/secured_pdu.hpp>
#include <vanetza/geonet/serialization_buffer.hpp>

namespace vanetza
{
namespace geonet
{

SecuredPdu::SecuredPdu(const Pdu& pdu) :
    common(pdu.common()), extended(pdu.extended_variant())
{
}

void serialize(const SecuredPdu& pdu, OutputArchive& ar)
{
    serialize(pdu.common, ar);
    serialize(pdu.extended, ar);
}

} // namespace geonet

namespace convertible
{

byte_buffer_impl<geonet::SecuredPdu>::byte_buffer_impl(const geonet::SecuredPdu& _pdu) :
    pdu(_pdu)
{
}

void byte_buffer_impl<geonet::SecuredPdu>::convert(ByteBuffer& buffer) const
{
    geonet::serialize_into_buffer(pdu, buffer);
}

std::size_t byte_buffer_impl<geonet::SecuredPdu>::size() const
{
    using namespace geonet;
    return CommonHeader::length_bytes + get_length(pdu.extended);
}

std::unique_ptr<byte_buffer> byte_buffer_impl<geonet::SecuredPdu>::duplicate() const
{
    return std::unique_ptr<byte_buffer_impl> { new byte_buffer_impl(*this) };
}

} // namespace convertible
} // namespace vanetza
