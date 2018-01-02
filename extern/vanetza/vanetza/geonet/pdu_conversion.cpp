#include "pdu_conversion.hpp"
#include "serialization_buffer.hpp"
#include <cassert>

namespace vanetza
{
namespace convertible
{

typedef std::unique_ptr<vanetza::geonet::Pdu> PduPtr;

void byte_buffer_impl<PduPtr>::convert(ByteBuffer& dest) const
{
    assert(m_pdu);
    dest.clear();
    geonet::serialize_into_buffer(*m_pdu, dest);
}

std::size_t byte_buffer_impl<PduPtr>::size() const
{
    assert(m_pdu);
    return get_length(*m_pdu);
}

std::unique_ptr<byte_buffer> byte_buffer_impl<PduPtr>::duplicate() const
{
    assert(m_pdu);
    PduPtr duplicate { m_pdu->clone() };
    std::unique_ptr<byte_buffer> result {
        new byte_buffer_impl<PduPtr>(std::move(duplicate))
    };
    return result;
}

} // namespace convertible

namespace geonet
{

Pdu* pdu_cast(ByteBufferConvertible& convertible)
{
    using convertible_pdu_t = convertible::byte_buffer_impl<std::unique_ptr<Pdu>>;
    auto convertible_pdu = dynamic_cast<convertible_pdu_t*>(convertible.ptr());
    return convertible_pdu ? convertible_pdu->m_pdu.get() : nullptr;
}

} // namespace geonet
} // namespace vanetza

