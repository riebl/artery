#include <vanetza/security/length_coding.hpp>
#include <vanetza/security/serialization.hpp>
#include <cassert>

namespace vanetza
{
namespace security
{

void serialize_length(OutputArchive& ar, size_t length)
{
    ByteBuffer buf;
    buf = encode_length(length);
    for (auto it = buf.begin(); it != buf.end(); it++) {
        ar << *it;
    }
}

size_t deserialize_length(InputArchive& ar)
{
    ByteBuffer buf(1);
    ar >> buf[0];
    const size_t leading = count_leading_ones(buf[0]);
    buf.resize(leading + 1);
    for (size_t c = 1; c <= leading; ++c) {
        ar >> buf[c];
    }
    auto tup = decode_length(buf);
    assert(tup);
    assert(std::get<0>(*tup) == buf.end());
    return std::get<1>(*tup);
}

} // namespace security
} // namespace vanetza
