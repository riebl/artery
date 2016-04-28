#include <gtest/gtest.h>
#include <vanetza/common/byte_sequence.hpp>
#include <vanetza/security/header_field.hpp>
#include <vanetza/security/tests/check_header_field.hpp>
#include <vanetza/security/tests/check_signature.hpp>
#include <vanetza/security/tests/serialization.hpp>
#include <algorithm>

using namespace vanetza::security;
using namespace vanetza;

TEST(HeaderField, Serialize)
{
    std::list<HeaderField> list;

    std::list<Certificate> certificates;
    for (unsigned i = 0; i < 2; ++i) {
        auto rand_gen = random_byte_generator(i + 8 * 3);
        Certificate cert;
        HashedId8 cert_digest;
        std::generate(cert_digest.begin(), cert_digest.end(), rand_gen);
        cert.signer_info = cert_digest;
        cert.subject_info = { SubjectType::Enrollment_Credential, random_byte_sequence(28, i) };
        cert.signature = create_random_ecdsa_signature(i + 8);
        certificates.push_back(cert);
    }
    list.push_back(SignerInfo { certificates });

    list.push_back(Time64 { 983 });

    Time64WithStandardDeviation time_dev;
    time_dev.log_std_dev = 1;
    time_dev.time64 = 2000;
    list.push_back(time_dev);

    list.push_back(Time32 { 434 });

    ThreeDLocation loc;
    loc.latitude.from_value(838);
    loc.longitude.from_value(37);
    loc.elevation = {{ 83, 17 }};
    list.push_back(loc);

    std::list<HashedId3> hashed;
    for (unsigned i = 0; i < 3; ++i) {
        HashedId3 id;
        std::generate(id.begin(), id.end(), random_byte_generator(i + 8943));
        hashed.push_back(id);
    }
    list.push_back(hashed);

    list.push_back(uint16_t { 43 });

    Nonce nonce;
    std::generate(nonce.begin(), nonce.end(), random_byte_generator(22));
    list.push_back(EncryptionParameter { nonce });

    std::list<RecipientInfo> recipients;
    for (unsigned i = 0; i < 2; ++i) {
        const std::size_t length = field_size(PublicKeyAlgorithm::Ecies_Nistp256);
        auto rand_gen = random_byte_generator(i + 93);
        RecipientInfo info;
        EciesEncryptedKey key;
        std::generate(info.cert_id.begin(), info.cert_id.end(), rand_gen);
        key.c = random_byte_sequence(field_size(SymmetricAlgorithm::Aes128_Ccm), rand_gen());
        std::generate(key.t.begin(), key.t.end(), rand_gen);
        key.v = Uncompressed {
            random_byte_sequence(length, rand_gen()),
            random_byte_sequence(length, rand_gen()) };
        info.enc_key = key;
        recipients.push_back(info);
    }
    list.push_back(recipients);

    check(list, serialize_roundtrip(list));
}

TEST(HeaderField, WebValidator_SecuredMessage3_adapted)
{
    const char str[] =
        "810280020201A8ED6DF65B0E6D6A010080940000040209B0434163CCBAFDD34A45333E418FB96C"
        "05BBE0E7E1D755D40D0B4BBE8DA508EC2F2723B7ADF0F27C39F3AECFF0783C196F9961F8821E6294"
        "375D9294CD6A01000452113CE698DB081491675DF8FFE81C23EA5D0071B2D2BF0E0DA4ADA0CDA582"
        "59CA5D999200B6565E194EDAB8BD3DCA863F2DDF39C13E7A0375ECE2566C5EB8C60200210AC04080"
        "0101C0408101010F01099EB20109B1270003040100960000008DA1F3F9F35E04C3DE77D7438988A8"
        "D57EBE44DAA021A4269E297C177C9CFE458E128EC290785D6631961625020943B6D87DAA54919A98"
        "F7865709929A7C6E480000009373CF482D40050002";
    std::list<HeaderField> list;
    const size_t deserialize_length = deserialize_from_hexstring(str, list);
    EXPECT_EQ(258, deserialize_length);
    EXPECT_EQ(258, get_size(list));
    EXPECT_EQ(3, list.size());
}
