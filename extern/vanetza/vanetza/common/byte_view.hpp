#ifndef BYTE_VIEW_HPP_TXN2ISMB
#define BYTE_VIEW_HPP_TXN2ISMB

#include <vanetza/common/byte_buffer.hpp>
#include <boost/range/iterator_range.hpp>
#include <memory>

namespace vanetza
{

// forward declaration
class ByteBufferConvertible;

/**
 * byte_view_range fulfills the range concept and provides a view of contiguous bytes
 * \note private inheritance is used to prevent object slicing
 */
class byte_view_range : private boost::iterator_range<ByteBuffer::const_iterator>
{
public:
    /**
     * Construct new view from iterator pair.
     * \param begin begin iterator of view
     * \param end end iterator of view
     * \note View is valid as long as passed iterators are valid
     */
    byte_view_range(const ByteBuffer::const_iterator&, const ByteBuffer::const_iterator&);

    /**
     * Create new view and take ownership of passed buffer
     * \param buffer pass buffer via rvalue
     * \note View is valid without limitation
     */
    byte_view_range(ByteBuffer&&);

    /**
     * Get pointer to start of contiguous buffer memory
     * \return pointer (can be nullptr)
     */
    ByteBuffer::const_pointer data() const;

    /**
     * Access a certain byte within range
     * \param pos byte position within [0; size()[
     * \note Override implementation by boost::iterator_range
     * \return byte value
     */
    ByteBuffer::value_type operator[](size_type) const;

    // make several funtions from boost::iterator_range accessible
    using boost::iterator_range<ByteBuffer::const_iterator>::size;
    using boost::iterator_range<ByteBuffer::const_iterator>::begin;
    using boost::iterator_range<ByteBuffer::const_iterator>::end;

private:
    ByteBuffer buffer;
};

/**
 * Create a byte view based on various byte buffer representations.
 * View is valid at least as long as passed arguments are valid
 * \param byte buffer or byte buffer convertible
 * \return byte view representing passed byte buffer
 */
byte_view_range create_byte_view(const ByteBuffer&);
byte_view_range create_byte_view(const ByteBufferConvertible&);

} // namespace vanetza

#endif /* BYTE_VIEW_HPP_TXN2ISMB */

