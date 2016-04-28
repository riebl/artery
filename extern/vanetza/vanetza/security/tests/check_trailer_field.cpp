#include <gtest/gtest.h>
#include <vanetza/security/tests/check_signature.hpp>
#include <vanetza/security/tests/check_trailer_field.hpp>
#include <vanetza/security/tests/check_visitor.hpp>
#include <boost/mpl/size.hpp>
#include <boost/variant/get.hpp>

namespace vanetza
{
namespace security
{

void check(const TrailerField& expected, const TrailerField& actual)
{
    static_assert(boost::mpl::size<typename TrailerField::types>::value == 1,
            "Simple check works only for TrailerField variant with one possible type");
    check(boost::get<Signature>(expected), boost::get<Signature>(actual));
}

} // namespace security
} // namespace vanetza

