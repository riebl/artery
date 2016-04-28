#include <gtest/gtest.h>
#include <vanetza/common/byte_buffer_source.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <istream>

using namespace vanetza;

TEST(ByteBufferSource, read) {
    const ByteBuffer buf = { 0x01, 0x11, 0x12, 0x22, 0x23, 0x33, 0x34, 0x44 };
    byte_buffer_source source(buf);
    boost::iostreams::stream_buffer<byte_buffer_source> stream(source);

    std::istream is(&stream);
    ASSERT_TRUE(is.good());

    char read = '\0';
    unsigned read_bytes = 0;
    for (uint8_t byte : buf) {
        is >> read;
        EXPECT_NE(byte, '\0');
        EXPECT_EQ(byte, read);
        ++read_bytes;
    }
    EXPECT_EQ(read_bytes, 8);

    is >> read;
    EXPECT_TRUE(is.eof());
}

