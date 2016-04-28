#include <gtest/gtest.h>
#include <vanetza/security/tests/check_region.hpp>
#include <vanetza/security/tests/check_validity_restriction.hpp>
#include <vanetza/security/tests/check_visitor.hpp>
#include <boost/format.hpp>

namespace vanetza
{
namespace security
{

void check(EndValidity expected, EndValidity actual)
{
    SCOPED_TRACE("EndValidity");
    EXPECT_EQ(expected, actual);
}

void check(const StartAndEndValidity& expected, const StartAndEndValidity& actual)
{
    SCOPED_TRACE("StartAndEndValidity");
    EXPECT_EQ(expected.start_validity, actual.start_validity);
    EXPECT_EQ(expected.end_validity, actual.end_validity);
}

void check(const StartAndDurationValidity& expected, const StartAndDurationValidity& actual)
{
    SCOPED_TRACE("StartAndDurationValidity");
    EXPECT_EQ(expected.start_validity, actual.start_validity);
    EXPECT_EQ(expected.duration.raw(), actual.duration.raw());
}

void check(const ValidityRestriction& expected, const ValidityRestriction& actual)
{
    ASSERT_EQ(get_type(expected), get_type(actual));
    boost::apply_visitor(check_visitor<ValidityRestriction>(), expected, actual);
}

} // namespace security
} // namespace vanetza

