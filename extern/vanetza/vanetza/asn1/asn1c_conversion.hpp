#ifndef ASN1C_CONVERSION_HPP_9E5QN6UC
#define ASN1C_CONVERSION_HPP_9E5QN6UC

#include <vanetza/asn1/asn1c_wrapper.hpp>
#include <vanetza/common/byte_buffer_convertible.hpp>
#include <memory>
#include <type_traits>

namespace vanetza
{
namespace convertible
{

template<class T>
struct byte_buffer_impl : public byte_buffer
{
public:
    using wrapper_type = T;
    static_assert(
        std::is_base_of<asn1::asn1c_wrapper<typename T::asn1c_type>, T>::value,
        "Only asn1c_wrapper derivates are supported");

    byte_buffer_impl(wrapper_type&& t) :
        m_wrapper(new wrapper_type(std::move(t)))
    {
    }

    void convert(ByteBuffer& buffer) const override
    {
        buffer = m_wrapper->encode();
    }

    std::unique_ptr<byte_buffer> duplicate() const override
    {
        return std::unique_ptr<byte_buffer> {
            new byte_buffer_impl { m_wrapper }
        };
    }

    std::size_t size() const override
    {
        return m_wrapper->size();
    }

    const std::shared_ptr<wrapper_type>& wrapper() const
    {
        return m_wrapper;
    }

private:
    byte_buffer_impl(const std::shared_ptr<wrapper_type>& other) :
        m_wrapper(other)
    {
    }

    std::shared_ptr<wrapper_type> m_wrapper;
};

} // namespace convertible
} // namespace vanetza

#endif /* ASN1C_CONVERSION_HPP_9E5QN6UC */

