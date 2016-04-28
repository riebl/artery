#include <gtest/gtest.h>
#include <vanetza/security/tests/check_basic_elements.hpp>
#include <vanetza/security/tests/check_header_field.hpp>
#include <vanetza/security/tests/check_visitor.hpp>

namespace vanetza
{
namespace security
{

void check(const HeaderField& expected, const HeaderField& actual)
{
    ASSERT_EQ(get_type(expected), get_type(actual));
    boost::apply_visitor(check_visitor<HeaderField>(), expected, actual);
}

} // namespace security
} // namespace vanetza
