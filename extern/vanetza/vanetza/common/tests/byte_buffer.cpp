#include <gtest/gtest.h>
#include <vanetza/common/byte_buffer.hpp>
#include <cstring>

using namespace vanetza;

struct A
{
    char b[10];
};

TEST(ByteBuffer, buffer_cast) {
    ByteBuffer buf = { 'A', ' ', 't', 'e', 's', 't', ' ', 'b', 'u', 'f', 'f', 'e', 'r' };
    ASSERT_GE(buf.size(), sizeof(A));
    A* a = buffer_cast<A>(buf);
    ASSERT_NE(nullptr, a);
    EXPECT_EQ(a->b[0], 'A');
    EXPECT_EQ(a->b[4], 's');
    EXPECT_EQ(a->b[9], 'f');
}

TEST(ByteBuffer, buffer_copy) {
    A a;
    strcpy(a.b, "123456789");
    auto copy = buffer_copy(a);
    ASSERT_EQ(copy.size(), 10);
    EXPECT_EQ(copy[0], '1');
    EXPECT_EQ(copy[3], '4');
    EXPECT_EQ(copy[8], '9');
    EXPECT_EQ(copy[9], '\0');

    a.b[0] = 'X';
    EXPECT_EQ(copy[0], '1');
}

