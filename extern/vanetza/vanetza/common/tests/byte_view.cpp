#include <gtest/gtest.h>
#include <vanetza/common/byte_view.hpp>
#include <vanetza/common/byte_buffer_convertible.hpp>

using namespace vanetza;

TEST(ByteView, range_buffer) {
    ByteBuffer buffer = {1, 2, 3, 4, 5, 6, 7, 8};
    byte_view_range view = create_byte_view(buffer);
    ASSERT_EQ(buffer.size(), view.size());
    EXPECT_EQ(1, *view.begin());
    EXPECT_EQ(3, view[2]);
}

TEST(ByteView, range_convertible) {
    byte_view_range view = create_byte_view(ByteBuffer());
    {
        std::string buffer = "temporary data";
        ByteBufferConvertible convertible { std::move(buffer) };
        // view should now own a byte buffer copy of original string
        view = create_byte_view(convertible);
    }

    ASSERT_EQ(14, view.size());
    EXPECT_EQ('t', view[0]);
    EXPECT_EQ('y', view[8]);
    EXPECT_EQ('d', view[10]);
}
