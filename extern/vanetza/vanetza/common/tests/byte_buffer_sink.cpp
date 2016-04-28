#include <gtest/gtest.h>
#include <vanetza/common/byte_buffer_sink.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <ostream>
#include <string>

using namespace vanetza;

TEST(ByteBufferSink, write) {
    ByteBuffer buf;
    byte_buffer_sink sink(buf);
    boost::iostreams::stream_buffer<byte_buffer_sink> stream(sink);
    ASSERT_TRUE(buf.empty());

    std::ostream os(&stream);
    ASSERT_TRUE(os.good());

    const std::string data("Hello World!");
    os << data;
    os.flush();
    EXPECT_EQ(buf.size(), 12);

    for (unsigned i = 0; i < buf.size(); ++i) {
        EXPECT_EQ(buf[i], data[i]);
    }

    EXPECT_TRUE(os.good());
}

