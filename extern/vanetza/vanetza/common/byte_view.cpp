#include <vanetza/common/byte_view.hpp>
#include <vanetza/common/byte_buffer_convertible.hpp>
#include <cassert>
#include <limits>

namespace vanetza
{

byte_view_range::byte_view_range(
        const ByteBuffer::const_iterator& begin,
        const ByteBuffer::const_iterator& end) :
    iterator_range(begin, end)
{
}

byte_view_range::byte_view_range(ByteBuffer&& _buffer) :
    iterator_range(_buffer.begin(), _buffer.end()), buffer(std::move(_buffer))
{
}

ByteBuffer::const_pointer byte_view_range::data() const
{
    auto begin = this->begin();
    return &(*begin);
}

ByteBuffer::value_type byte_view_range::operator[](size_type pos) const
{
    assert(!std::numeric_limits<size_type>::is_signed || pos >= 0);
    assert(pos < size());
    return data()[pos];
}

byte_view_range create_byte_view(const ByteBuffer& buffer)
{
    return byte_view_range { buffer.begin(), buffer.end() };
}

byte_view_range create_byte_view(const ByteBufferConvertible& convertible)
{
    ByteBuffer buffer;
    convertible.convert(buffer);
    return byte_view_range { std::move(buffer) };
}

} // namespace vanetza
