#include "byte_buffer_sink.hpp"
#include <algorithm>
#include <iterator>

namespace vanetza
{

byte_buffer_sink::byte_buffer_sink(ByteBuffer& buffer) : m_buffer(buffer)
{
}

std::streamsize byte_buffer_sink::write(const char_type* s, std::streamsize n)
{
    std::copy(s, s + n, std::back_inserter(m_buffer));
    return n;
}

} // namespace vanetza

