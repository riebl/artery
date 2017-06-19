#include <gtest/gtest.h>
#include <vanetza/common/byte_order.hpp>
#include <sstream>

using namespace vanetza;

using le_type = EndianType<uint32_t, ByteOrder::LittleEndian>;
using be_type = EndianType<uint32_t, ByteOrder::BigEndian>;

TEST(ByteOrder, equality)
{
    EndianType<int, ByteOrder::BigEndian> a(3);
    EndianType<int, ByteOrder::BigEndian> b(3);
    EXPECT_EQ(a, b);

    a = host_cast(static_cast<int>(4));
    EXPECT_NE(a, b);
}

TEST(ByteOrder, access)
{
    le_type a = host_cast<uint32_t>(0x12345678);
    be_type b = host_cast<uint32_t>(0x12345678);
    EXPECT_NE(a.get(), b.get());
    EXPECT_EQ(a.net(), b.net());
    EXPECT_EQ(a.host(), b.host());
}

TEST(ByteOrder, less)
{
    uint8be_t a { 3 };
    uint8be_t b { 5 };
    uint8be_t c { 5 };
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < c);
    EXPECT_FALSE(b < a);
}

TEST(ByteOrder, hash)
{
    le_type a = host_cast<uint32_t>(0x12345678);
    be_type b = host_cast<uint32_t>(0x12345678);
    be_type c = host_cast<uint32_t>(0x78563412);
    EXPECT_EQ(std::hash<le_type>()(a), std::hash<le_type>()(a));
    EXPECT_EQ(std::hash<be_type>()(b), std::hash<be_type>()(b));
    EXPECT_NE(std::hash<be_type>()(b), std::hash<be_type>()(c));
}

