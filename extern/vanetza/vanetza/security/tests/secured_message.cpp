#include <gtest/gtest.h>
#include <vanetza/security/secured_message.hpp>
#include <vanetza/security/tests/check_secured_message.hpp>
#include <vanetza/security/tests/check_signature.hpp>
#include <vanetza/security/tests/serialization.hpp>

using namespace vanetza;
using namespace vanetza::security;

// Note: WebValidator refers to https://werkzeug.dcaiti.tu-berlin.de/etsi/ts103097/

TEST(SecuredMessage, Serialization)
{
    SecuredMessage m;

    m.header_fields.push_back(Time64 { 0x4711 });
    m.payload = {PayloadType::Unsecured, CohesivePacket({ 5, 10, 15, 25, 40 }, OsiLayer::Application)};
    std::list<TrailerField> list;
    list.push_back(Signature { create_random_ecdsa_signature(44) });
    list.push_back(Signature { create_random_ecdsa_signature(45) });
    m.trailer_fields = list;

    check(m, serialize_roundtrip(m));
}

TEST(SecuredMessage, header_field_extractor)
{
    SecuredMessage m;
    auto empty = m.header_field(HeaderFieldType::Generation_Time);
    EXPECT_FALSE(empty);

    m.header_fields.push_back(Time64 { 25 });
    m.header_fields.push_back(IntX { 23 });

    auto time = m.header_field(HeaderFieldType::Generation_Time);
    ASSERT_TRUE(time);
    EXPECT_EQ(25, boost::get<Time64>(*time));

    auto msg = m.header_field(HeaderFieldType::Its_Aid);
    ASSERT_TRUE(msg);
    EXPECT_EQ(23, boost::get<IntX>(*msg).get());

    // field modifications should pass through
    boost::get<Time64>(*time) = 26;
    auto time2 = m.header_field(HeaderFieldType::Generation_Time);
    ASSERT_TRUE(time2);
    EXPECT_EQ(Time64 { 26 }, boost::get<Time64>(*time2));
}

TEST(SecuredMessage, trailer_field_extractor)
{
    SecuredMessage m;
    auto empty = m.trailer_field(TrailerFieldType::Signature);
    EXPECT_FALSE(!!empty);

    m.trailer_fields.push_back(Signature {});
    m.trailer_fields.push_back(Signature {});
    auto first = m.trailer_field(TrailerFieldType::Signature);
    ASSERT_TRUE(!!first);
    EXPECT_EQ(&m.trailer_fields.front(), first);
}

TEST(SecuredMessage, WebValidator_Serialize_SecuredMessageV2_1)
{
    // SecuredMessage/v1 from FOKUS WebValidator adapted for v2
    const char str[] =
        "02810180020201A8ED6DF65B0E6D6A0100809400000418929DB6A9E452223062C52028E956BF98"
        "74E0A40D21D5F9F56564F39C5DD187C922F2E5F0630373879A43393373B9F6205BF01FBD9C1F1131"
        "65C291C376F535010004EABA91A915D81807E910FD292D99DF8B401EED88CF7F031412D5ED9905F9"
        "996469798C412FC8F7237A3AB3469795E2DEF5E1B783EA4F6B6A2359D21772B2EA9D0200210AC040"
        "800101C0408101010F01099EB20109B1270003040100960000004B2E6D0D0EE9BC4AD9CD087B601E"
        "9AF06031995443D652763455FBB794B33982889260740EF64CFA8C6808A58F98E06CE42A1E9C22A0"
        "785D7242647F7895ABFC0000009373931CD7580502011C983E690E5F6D755BD4871578A9427E7B"
        "C383903DC7DA3B560384013643010000FE8566BEA87B39E6411F80226E792D6E01E77B598F2BB1FC"
        "E7F2DD441185C07CEF0573FBFB9876B99FE811486F6F5D499E6114FC0724A67F8D71D2A897A7EB34";
    SecuredMessage m;
    deserialize_from_hexstring(str, m);

    EXPECT_EQ(358, get_size(m));
    EXPECT_EQ(2, m.protocol_version());

    EXPECT_EQ(3, m.header_fields.size());
    auto signer_info = m.header_field(HeaderFieldType::Signer_Info);
    ASSERT_TRUE(!!signer_info);
    auto generation_time = m.header_field(HeaderFieldType::Generation_Time);
    ASSERT_TRUE(!!generation_time);
    auto its_aid = m.header_field(HeaderFieldType::Its_Aid);
    ASSERT_TRUE(!!its_aid);
    EXPECT_EQ(IntX {2}, boost::get<IntX>(*its_aid));

    EXPECT_EQ(PayloadType::Signed, m.payload.type);
    EXPECT_EQ(28, size(m.payload.data, min_osi_layer(), max_osi_layer()));

    EXPECT_EQ(1, m.trailer_fields.size());
    EXPECT_TRUE(!!m.trailer_field(TrailerFieldType::Signature));
}

TEST(SecuredMessage, WebValidator_Serialize_SecuredMessageV2_2)
{
    // SecuredMessage/v1 from FOKUS WebValidator adapted for v2
    const char str[] =
        "02810180020201A8ED6DF65B0E6D6A01008094000004C4EC137145DD4F450145DE530CCA36E73A"
        "B3D87FC8275847CDAD8248C1CD20879BD6A8CB54EA9E05D3B41376CE2F24789AEF82836CA818D568"
        "ADF4A140E96E48010004D6C268EE68B5B8B387B2312B7E1D21CE0C366D251A32431508B96EB6A347"
        "9CCF96A8738F30ED451F00DA8DDE84367C7EB16727D14FF14F5DD8F9791FE0A12A640200210AC040"
        "800101C0408101010F01099EB20109B1270003040100960000001EB035FE8E51DCDD8558DE0BE9B8"
        "7895B36B420583A5C6B2B8B2EAB7F3D3C99163638FA025A0033D4BD80BBA02B8E3DE1B55766459D4"
        "94677AF24917E51B80AC0000009373CC5F22C805020120F29384759027349075829034707ABABA"
        "BABABAABAB98437985739845783974954301000081E7CDB6D2C741C1700822305C39E8E809622AF9"
        "FCA1C0786F762D08E80580C42F1FCC1D5499577210834C390BB4613E102DECB14F575A2820743DC9"
        "A66BBD7A";
    SecuredMessage m;
    deserialize_from_hexstring(str, m);
    check(m, serialize_roundtrip(m));
}

TEST(SecuredMessage, WebValidator_Serialize_SecuredMessageV2_3)
{
    // SecuredMessage/v1 from FOKUS WebValidator adapted for v2
    const char str[] =
        "02810180020201A8ED6DF65B0E6D6A010080940000040209B0434163CCBAFDD34A45333E418FB9"
        "6C05BBE0E7E1D755D40D0B4BBE8DA508EC2F2723B7ADF0F27C39F3AECFF0783C196F9961F8821E62"
        "94375D9294CD6A01000452113CE698DB081491675DF8FFE81C23EA5D0071B2D2BF0E0DA4ADA0CDA5"
        "8259CA5D999200B6565E194EDAB8BD3DCA863F2DDF39C13E7A0375ECE2566C5EB8C60200210AC040"
        "800101C0408101010F01099EB20109B1270003040100960000008DA1F3F9F35E04C3DE77D7438988"
        "A8D57EBE44DAA021A4269E297C177C9CFE458E128EC290785D6631961625020943B6D87DAA54919A"
        "98F7865709929A7C6E480000009373CF482D40050201080123456789ABCDEF43010000371423BB"
        "A0902D8AF2FB2226D73A7781D4D6B6772650A8BEE5A1AF198CEDABA2C9BF57540C629E6A1E629B88"
        "12AEBDDDBCAF472F6586F16C14B3DEFBE9B6ADB2";
    SecuredMessage m;
    deserialize_from_hexstring(str, m);
    check(m, serialize_roundtrip(m));
}

