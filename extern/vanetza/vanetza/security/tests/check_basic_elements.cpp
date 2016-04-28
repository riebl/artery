#include <vanetza/security/tests/check_basic_elements.hpp>

namespace vanetza
{
namespace security
{

void check(const Time64WithStandardDeviation& expected, const Time64WithStandardDeviation& actual)
{
    EXPECT_EQ(expected.time64, actual.time64);
    EXPECT_EQ(expected.log_std_dev, actual.log_std_dev);
}

void check(const IntX& expected, const IntX& actual)
{
    EXPECT_EQ(expected, actual);
}

} // namespace security
} // namespace vanetza
