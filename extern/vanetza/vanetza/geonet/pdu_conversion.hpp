#ifndef PDU_CONVERSION_HPP_XLCSI42E
#define PDU_CONVERSION_HPP_XLCSI42E

#include <vanetza/common/byte_buffer.hpp>
#include <vanetza/common/byte_buffer_convertible.hpp>
#include <vanetza/geonet/pdu.hpp>
#include <memory>

namespace vanetza
{
namespace convertible
{

template<>
struct byte_buffer_impl<std::unique_ptr<vanetza::geonet::Pdu>> : public byte_buffer
{
    byte_buffer_impl(std::unique_ptr<vanetza::geonet::Pdu> pdu) :
        m_pdu(std::move(pdu)) {}

    void convert(ByteBuffer& dest) const override;
    std::size_t size() const override;
    std::unique_ptr<byte_buffer> duplicate() const override;

    const std::unique_ptr<vanetza::geonet::Pdu> m_pdu;
};

} // namespace convertible

namespace geonet
{

/**
 * Fetch PDU from byte buffer convertible
 * \param conv source convertible
 * \return PDU pointer or nullptr if cast failed
 * */
Pdu* pdu_cast(ByteBufferConvertible& conv);

} // namespace geonet
} // namespace vanetza

#endif /* PDU_CONVERSION_HPP_XLCSI42E */

