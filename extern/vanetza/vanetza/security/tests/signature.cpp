#include <gtest/gtest.h>
#include <vanetza/common/byte_sequence.hpp>
#include <vanetza/security/public_key.hpp>
#include <vanetza/security/signature.hpp>
#include <vanetza/security/tests/check_signature.hpp>
#include <vanetza/security/tests/serialization.hpp>

using namespace vanetza::security;

TEST(Signature, serialize)
{
    Signature signature = create_random_ecdsa_signature(42);
    check(signature, serialize_roundtrip(signature));
}

TEST(Signature, WebValidator_Size)
{
    Signature sig;
    EcdsaSignature eSig;
    X_Coordinate_Only x;
    x.x = buffer_from_hexstring("8DA1F3F9F35E04C3DE77D7438988A8D57EBE44DAA021A4269E297C177C9CFE45");
    eSig.R = x;
    eSig.s = buffer_from_hexstring("8E128EC290785D6631961625020943B6D87DAA54919A98F7865709929A7C6E48");
    sig = eSig;

    EXPECT_EQ(66, get_size(sig));
}
