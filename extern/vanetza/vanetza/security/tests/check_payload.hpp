#ifndef CHECK_PAYLOAD_HPP_YNRGOKGC
#define CHECK_PAYLOAD_HPP_YNRGOKGC

#include <gtest/gtest.h>
#include <vanetza/common/byte_buffer.hpp>
#include <vanetza/common/serialization_buffer.hpp>
#include <vanetza/security/payload.hpp>

namespace vanetza
{
namespace security
{

inline void check(const PacketVariant& expected, const PacketVariant& actual)
{
    ByteBuffer expected_buf, actual_buf;
    serialize_into_buffer(expected, expected_buf);
    serialize_into_buffer(actual, actual_buf);
    EXPECT_EQ(expected_buf, actual_buf);
}

inline void check(const Payload& expected, const Payload& actual)
{
    EXPECT_EQ(expected.type, actual.type);
    check(expected.data, actual.data);
}

} // namespace security
} // namespace vanetza

#endif /* CHECK_PAYLOAD_HPP_YNRGOKGC */

