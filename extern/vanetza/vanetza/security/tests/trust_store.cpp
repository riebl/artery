#include <gtest/gtest.h>
#include <vanetza/security/certificate.hpp>
#include <vanetza/security/trust_store.hpp>
#include <vanetza/security/tests/serialization.hpp>

using namespace vanetza::security;

TEST(TrustStoreTest, find_by_id)
{
    const char str[] =
            "0200040C547275737465645F526F6F74808D000004F1817DD05116B855A853F80DB171A3A470D431"
            "70EA7EEFD8EF392D66ECEFBE501CEBA19963C9B6447574424FFF1BB89485743F4D09A72B715FC73C"
            "87E5F70A110101000441279A383B80C812B72B1A5F5C3C590E5041C634A1ADCC4CE58393CA046D3C"
            "619717AEF634F7D80D5F6A29FA7F86EBF823ACE0097A71EE0DF0793034B0D3797C02E0200224250B"
            "0114B12B03154E0D83030000007D12BADF99D7070BCB237ED1FA7A5D86FD47E6ABA8E616B35E95A2"
            "856FC6E26A493E1215BCEE8BEA18B8ED52FB240716C4D4EC7D7C0167F0F032CBB87DF611D9";

    Certificate c;
    deserialize_from_hexstring(str, c);

    std::vector<Certificate> trusted_certificates;
    trusted_certificates.push_back(c);

    TrustStore trust_store(trusted_certificates);

    HashedId8 id;
    std::vector<Certificate> matched_certificates;

    id = HashedId8 { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    matched_certificates = trust_store.find_by_id(id);
    EXPECT_EQ(0, matched_certificates.size());

    id = calculate_hash(c);
    matched_certificates = trust_store.find_by_id(id);
    EXPECT_EQ(1, matched_certificates.size());
}
