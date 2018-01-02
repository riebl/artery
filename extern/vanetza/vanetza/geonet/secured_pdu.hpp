#ifndef SECURED_PDU_HPP_TVERYI91
#define SECURED_PDU_HPP_TVERYI91

#include <vanetza/common/byte_buffer_convertible.hpp>
#include <vanetza/geonet/common_header.hpp>
#include <vanetza/geonet/extended_pdu.hpp>
#include <vanetza/geonet/header_variant.hpp>
#include <vanetza/geonet/serialization.hpp>

namespace vanetza
{
namespace geonet
{

/**
 * Secured PDU consists of GeoNetworking headers belonging to a secured message's payload
 */
class SecuredPdu
{
public:
    template<typename EXT>
    SecuredPdu(const ExtendedPdu<EXT>& pdu) :
        common(pdu.common()), extended(pdu.extended())
    {
    }

    SecuredPdu(const Pdu&);

    CommonHeader common;
    HeaderVariant extended;
};

void serialize(const SecuredPdu&, OutputArchive&);

/**
 * Cast secured PDU from byte buffer convertible
 * \param conv source convertible
 * \return secured PDU pointer or nullptr if cast failed
 */
SecuredPdu* secured_pdu_cast(ByteBufferConvertible&);

} // namespace geonet

namespace convertible
{

template<>
struct byte_buffer_impl<geonet::SecuredPdu> : public byte_buffer
{
    byte_buffer_impl(const geonet::SecuredPdu&);
    void convert(ByteBuffer&) const override;
    std::size_t size() const override;
    std::unique_ptr<byte_buffer> duplicate() const override;

    geonet::SecuredPdu pdu;
};

} // namespace convertible

} // namespace vanetza

#endif /* SECURED_PDU_HPP_TVERYI91 */

