#include <gtest/gtest.h>
#include <vanetza/security/subject_attribute.hpp>
#include <vanetza/security/tests/check_list.hpp>
#include <vanetza/security/tests/check_subject_attribute.hpp>
#include <vanetza/security/tests/serialization.hpp>

using namespace vanetza::security;

TEST(SubjectAttribute, serialize)
{
    std::list<SubjectAttribute> list;
    list.push_back(create_random_verification_key(33));
    list.push_back(create_random_encryption_key(34));
    list.push_back(SubjectAssurance { 124 });
    std::list<IntX> its_aid;
    for (int i = 0; i < 4; ++i) {
        its_aid.push_back(create_random_its_aid(i + 66));
    }
    list.push_back(its_aid);
    std::list<ItsAidSsp> its_aid_ssp;
    for (int i = 0; i < 1; ++i) {
        its_aid_ssp.push_back(create_random_its_aid_ssp(i + 67));
    }
    list.push_back(its_aid_ssp);

    check(list, serialize_roundtrip(list));
}

TEST(SubjectAttribute, WebValidator_ItsAidSsp)
{
    std::list<ItsAidSsp> list;
    ItsAidSsp its1;
    its1.its_aid.set(16512);
    its1.service_specific_permissions = {0x01};
    list.push_back(its1);

    ItsAidSsp its2;
    its2.its_aid.set(16513);
    its2.service_specific_permissions = {0x01};
    list.push_back(its2);

    EXPECT_EQ(10, get_size(list));
    std::list<ItsAidSsp> other_list;
    deserialize_from_hexstring("0AC040800101C040810101", other_list);
    check(list, other_list);
}
