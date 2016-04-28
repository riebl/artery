#ifndef BYTE_BUFFER_SOURCE_HPP_6IBYOI0T
#define BYTE_BUFFER_SOURCE_HPP_6IBYOI0T

#include <vanetza/common/byte_buffer.hpp>
#include <boost/iostreams/categories.hpp>
#include <boost/range/iterator_range.hpp>

namespace vanetza
{

/**
 * Implementation of boost::iostreams' source device concept.
 * Allows to use a byte buffer as input stream.
 */
class byte_buffer_source
{
public:
    typedef char char_type;
    typedef boost::iostreams::source_tag category;
    typedef ByteBuffer::const_iterator iterator;
    typedef boost::iterator_range<iterator> range;

    static_assert(sizeof(char_type) == sizeof(ByteBuffer::value_type),
            "size mismatch of char_type and ByteBuffer::value_type");

    byte_buffer_source(const ByteBuffer& buffer);
    byte_buffer_source(range r);
    byte_buffer_source(iterator begin, iterator end);
    std::streamsize read(char_type* buf, std::streamsize n);

private:
    iterator m_begin;
    iterator m_end;
};

} // namespace vanetza

#endif /* BYTE_BUFFER_SOURCE_HPP_6IBYOI0T */

