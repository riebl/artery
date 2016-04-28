#include <vanetza/security/length_coding.hpp>
#include <cmath>
#include <iterator>
#include <list>

namespace vanetza
{
namespace security
{

std::size_t count_leading_ones(uint8_t v)
{
    std::size_t count = 0;
    while ((v & 0x80) != 0) {
        v <<= 1;
        ++count;
    }
    return count;
}

std::size_t length_coding_size(std::size_t length) {
    std::size_t size = 1;
    while ((length & ~0x7f) != 0) {
        // prefix enlongates by one additional leading "1" per shift
        length >>= 7; // shift by 7
        ++size;
    }
    return size;
}

ByteBuffer encode_length(std::size_t length)
{
    std::list<uint8_t> length_info;

    while (length != 0) {
        length_info.push_front(static_cast<uint8_t>(length));
        length >>= 8;
    }

    unsigned prefix_length = length_info.size();
    if (prefix_length == 0) {
        // Zero-size encoding
        length_info.push_back(0x00);
    }
    else {
        uint8_t prefix_mask = static_cast<int8_t>(0x80) >> ((prefix_length % 8) - 1);
        if ((length_info.front() & ~prefix_mask) != length_info.front()) {
            // additional byte needed for prefix
            length_info.push_front(prefix_mask);
        }
        else {
            // enough free bits available for prefix
            length_info.front() |= (prefix_mask << 1);
        }
        // Huge lengths have all bits set in leading prefix bytes
        length_info.insert(length_info.begin(), prefix_length / 8, 0xff);
    }

    return ByteBuffer(length_info.begin(), length_info.end());
}

boost::iterator_range<ByteBuffer::const_iterator> decode_length_range(const ByteBuffer& buffer)
{
    auto range = boost::make_iterator_range(buffer.begin(), buffer.end());
    auto payload = decode_length(buffer);
    if (payload) {
        ByteBuffer::const_iterator start;
        ByteBuffer::difference_type length = 0;
        std::tie(start, length) = payload.get();
        if (std::distance(start, buffer.end()) >= length) {
            ByteBuffer::const_iterator stop = start;
            std::advance(stop, length);
            range = boost::make_iterator_range(start, stop);
        }
    }

    return range;
}

boost::optional<std::tuple<ByteBuffer::const_iterator, std::size_t>> decode_length(
    const ByteBuffer& buffer)
{
    boost::optional<std::tuple<ByteBuffer::const_iterator, std::size_t>> result;

    if (!buffer.empty()) {
        std::size_t additional_prefix = count_leading_ones(buffer.front());
        static const std::size_t additional_bytes_max = std::min(sizeof(std::size_t) - 1,
            static_cast<std::size_t>(7));

        if (additional_prefix <= additional_bytes_max && buffer.size() >= additional_prefix) {
            uint8_t prefix_mask = static_cast<int8_t>(0x80) >> additional_prefix;
            std::size_t length = buffer.front() & ~prefix_mask;
            for (std::size_t i = 1; i <= additional_prefix; ++i) {
                length <<= 8;
                length |= buffer[i];
            }

            auto start = buffer.begin();
            std::advance(start, additional_prefix + 1);
            result = std::make_tuple(start, length);
        }
    }

    return result;
}

} // namespace security
} // namespace vanextza
