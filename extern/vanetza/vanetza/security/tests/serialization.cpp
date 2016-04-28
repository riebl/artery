#include <vanetza/security/tests/serialization.hpp>
#include <boost/algorithm/hex.hpp>

namespace vanetza
{
namespace security
{

ByteBuffer buffer_from_hexstring(const char* string)
{
    ByteBuffer buf;
    boost::algorithm::unhex(string, back_inserter(buf));
    return buf;
}

} // namespace security
} // namespace vanetza
