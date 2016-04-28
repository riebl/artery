#include "byte_buffer_convertible.hpp"
#include <algorithm>
#include <iterator>

namespace vanetza
{
namespace convertible
{

std::unique_ptr<byte_buffer> byte_buffer::duplicate() const
{
    ByteBuffer duplicate;
    this->convert(duplicate);
    std::unique_ptr<byte_buffer> result {
        new byte_buffer_impl<ByteBuffer>(std::move(duplicate))
    };
    return result;
}

byte_buffer_impl<std::string>::byte_buffer_impl(const std::string& str) : m_buffer(str) {}
byte_buffer_impl<std::string>::byte_buffer_impl(std::string&& str) : m_buffer(std::move(str)) {}

void byte_buffer_impl<std::string>::convert(ByteBuffer& buffer) const
{
    buffer.clear();
    std::copy(m_buffer.begin(), m_buffer.end(), std::back_inserter(buffer));
}

std::size_t byte_buffer_impl<std::string>::size() const
{
    return m_buffer.size();
}

std::unique_ptr<byte_buffer> byte_buffer_impl<std::nullptr_t>::duplicate() const
{
    return std::unique_ptr<byte_buffer> { new byte_buffer_impl<std::nullptr_t>() };
}

} // namespace convertible

ByteBufferConvertible::ByteBufferConvertible(const ByteBufferConvertible& other) :
    m_wrapper(other.m_wrapper->duplicate())
{
}

ByteBufferConvertible& ByteBufferConvertible::operator=(const ByteBufferConvertible& other)
{
    m_wrapper = other.m_wrapper->duplicate();
    return *this;
}

} // namespace vanetza

