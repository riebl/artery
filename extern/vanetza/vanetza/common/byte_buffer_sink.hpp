#ifndef BYTE_BUFFER_SINK_HPP_XQSBHPDQ
#define BYTE_BUFFER_SINK_HPP_XQSBHPDQ

#include <vanetza/common/byte_buffer.hpp>
#include <boost/iostreams/categories.hpp>
#include <ios>

namespace vanetza
{

class byte_buffer_sink
{
public:
    typedef char char_type;
    typedef boost::iostreams::sink_tag category;
    static_assert(sizeof(char_type) == sizeof(ByteBuffer::value_type),
            "size mismatch of char_type and ByteBuffer::value_type");

    byte_buffer_sink(ByteBuffer& buffer);
    std::streamsize write(const char_type* s, std::streamsize n);

private:
    ByteBuffer& m_buffer;
};

} // namespace vanetza

#endif /* BYTE_BUFFER_SINK_HPP_XQSBHPDQ */

