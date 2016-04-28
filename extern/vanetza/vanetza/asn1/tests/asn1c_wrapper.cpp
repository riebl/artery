#include <gtest/gtest.h>
#include <vanetza/asn1/asn1c_wrapper.hpp>
#include <vanetza/asn1/gen/VanetzaTest.h>

using namespace vanetza::asn1;
typedef vanetza::asn1::asn1c_wrapper<VanetzaTest_t> test_wrapper;

TEST(asn1c_wrapper, create) {
    EXPECT_NO_THROW({
        test_wrapper wrapper(asn_DEF_VanetzaTest);
    });
}

TEST(asn1c_wrapper, constants) {
    EXPECT_EQ(42, VanetzaTest__field_magicValue);
}

TEST(asn1c_wrapper, size) {
    test_wrapper wrapper(asn_DEF_VanetzaTest);
    OCTET_STRING_fromString(&wrapper->string, "1234");
    EXPECT_EQ(wrapper.size(), 5);
}

TEST(asn1c_wrapper, dereferencing) {
    test_wrapper wrapper(asn_DEF_VanetzaTest);
    ASSERT_NE(wrapper->field, 3);
    wrapper->field = 3;
    EXPECT_EQ((*wrapper).field, 3);
}

TEST(asn1c_wrapper, copy) {
    test_wrapper wrapper_orig(asn_DEF_VanetzaTest);
    wrapper_orig->field = 5;
    test_wrapper wrapper_copy = wrapper_orig;
    EXPECT_EQ(wrapper_copy->field, 5);
    wrapper_copy->field = 6;
    EXPECT_EQ(wrapper_orig->field, 5);
}

TEST(asn1c_wrapper, validate) {
    test_wrapper wrapper(asn_DEF_VanetzaTest);
    OCTET_STRING_fromString(&wrapper->string, "1234");
    EXPECT_TRUE(wrapper.validate());
    OCTET_STRING_fromString(&wrapper->string, "ABCD"); // non-numerics
    EXPECT_FALSE(wrapper.validate());
    std::string msg;
    EXPECT_FALSE(wrapper.validate(msg));
    EXPECT_FALSE(msg.empty());
}

TEST(asn1c_wrapper, encode) {
    test_wrapper wrapper(asn_DEF_VanetzaTest);
    OCTET_STRING_fromString(&wrapper->string, "1234");
    wrapper->field = 8;
    vanetza::ByteBuffer buf = wrapper.encode();
    EXPECT_EQ(wrapper.size(), buf.size());
    // No idea if this is the correct ASN.1 PER equivalent
    EXPECT_EQ(vanetza::ByteBuffer({ 0x04, 0x02, 0x11, 0xA2, 0x80 }), buf);
}

TEST(asn1c_wrapper, decode_valid) {
    test_wrapper wrapper(asn_DEF_VanetzaTest);
    const vanetza::ByteBuffer buffer { 0x04, 0x02, 0x11, 0xA2, 0x80 };
    bool result = wrapper.decode(buffer);
    ASSERT_TRUE(result);
    EXPECT_EQ(8, wrapper->field);
    ASSERT_EQ(4, wrapper->string.size);
    EXPECT_STREQ("1234", (const char*)(wrapper->string.buf));
}

TEST(asn1c_wrapper, decode_invalid) {
    test_wrapper wrapper(asn_DEF_VanetzaTest);
    const vanetza::ByteBuffer buffer { 0x12 };
    bool result = wrapper.decode(buffer);
    // should have failed because of short buffer
    ASSERT_FALSE(result);
}
