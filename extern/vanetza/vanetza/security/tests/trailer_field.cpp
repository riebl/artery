#include <gtest/gtest.h>
#include <vanetza/common/byte_sequence.hpp>
#include <vanetza/security/trailer_field.hpp>
#include <vanetza/security/tests/check_list.hpp>
#include <vanetza/security/tests/check_signature.hpp>
#include <vanetza/security/tests/check_trailer_field.hpp>
#include <vanetza/security/tests/serialization.hpp>

using namespace vanetza;
using namespace vanetza::security;

TEST(TrailerField, Serialization)
{
    std::list<TrailerField> list;
    EcdsaSignature a = create_random_ecdsa_signature(8);
    EcdsaSignature b = create_random_ecdsa_signature(9);
    list.push_back(TrailerField { a });
    list.push_back(TrailerField { b });

    check(list, serialize_roundtrip(list));
}

TEST(TrailerField, WebValidator_Size)
{
    TrailerField field;
    Signature sig;
    EcdsaSignature ecdsa;
    EccPoint point;
    X_Coordinate_Only x;
    x.x = buffer_from_hexstring("371423BBA0902D8AF2FB2226D73A7781D4D6B6772650A8BEE5A1AF198CEDABA2");
    point = x;
    ecdsa.R = point;
    ecdsa.s = buffer_from_hexstring("C9BF57540C629E6A1E629B8812AEBDDDBCAF472F6586F16C14B3DEFBE9B6ADB2");
    sig = ecdsa;
    field = sig;

    std::list<TrailerField> list;
    list.push_back(field);

    EXPECT_EQ(67, get_size(list));
    EXPECT_EQ(67, get_size(field));
}
