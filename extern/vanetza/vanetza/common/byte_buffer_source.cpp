#include "byte_buffer_source.hpp"
#include <algorithm>

namespace vanetza
{

byte_buffer_source::byte_buffer_source(const ByteBuffer& buffer) :
    m_begin(buffer.cbegin()), m_end(buffer.cend()) {}

byte_buffer_source::byte_buffer_source(range r) :
    m_begin(r.begin()), m_end(r.end()) {}

byte_buffer_source::byte_buffer_source(iterator begin, iterator end) :
    m_begin(begin), m_end(end) {}

std::streamsize byte_buffer_source::read(char_type* buf, std::streamsize n)
{
    if (m_begin == m_end) {
        return -1;
    } else {
        std::streamsize remaining = std::distance(m_begin, m_end);
        std::streamsize consume = std::min(remaining, n);
        std::copy(m_begin, m_begin + consume, buf);
        std::advance(m_begin, consume);
        return consume;
    }
}

} // namespace vanetza

