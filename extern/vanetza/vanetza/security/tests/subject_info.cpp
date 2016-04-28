#include <gtest/gtest.h>
#include <vanetza/common/byte_sequence.hpp>
#include <vanetza/security/subject_info.hpp>
#include <vanetza/security/tests/check_subject_info.hpp>
#include <vanetza/security/tests/serialization.hpp>

using namespace vanetza::security;

TEST(SubjectInfo, Serialization)
{
    SubjectInfo sub;
    sub.subject_type = SubjectType::Root_Ca;
    sub.subject_name = vanetza::random_byte_sequence(40);
    check(sub, serialize_roundtrip(sub));
}

TEST(SubjectInfo, WebValidator_Size)
{
    SubjectInfo info;
    info.subject_type = SubjectType::Authorization_Ticket;

    EXPECT_EQ(2, get_size(info));
}
