#include <gtest/gtest.h>
#include <vanetza/common/byte_sequence.hpp>
#include <vanetza/security/certificate.hpp>
#include <vanetza/security/signer_info.hpp>
#include <vanetza/security/tests/check_signature.hpp>
#include <vanetza/security/tests/check_signer_info.hpp>
#include <vanetza/security/tests/serialization.hpp>

using vanetza::random_byte_sequence;
using namespace vanetza::security;

TEST(SignerInfo, Serialization)
{
    std::list<Certificate> certificates;
    Certificate a;
    a.signature = create_random_ecdsa_signature(28);
    Certificate b;
    b.signature = create_random_ecdsa_signature(29);
    certificates.push_back(a);
    certificates.push_back(b);
    SignerInfo info { certificates };

    check(info, serialize_roundtrip(info));
}

TEST(SignerInfo, WebValidator_Size)
{
    SignerInfo info;
    HashedId8 id {{ 0xA8, 0xED, 0x6D, 0xF6, 0x5B, 0x0E, 0x6D, 0x6A }};
    info = id;

    EXPECT_EQ(9, get_size(info));
}
