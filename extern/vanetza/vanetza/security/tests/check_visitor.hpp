#ifndef CHECK_VISITOR_HPP_YJ7UPXCB
#define CHECK_VISITOR_HPP_YJ7UPXCB

#include <gtest/gtest.h>
#include <boost/variant/static_visitor.hpp>
#include <typeinfo>

namespace vanetza
{
namespace security
{

template<class VARIANT>
struct check_visitor : public boost::static_visitor<>
{
    template<typename R, typename S>
    void operator()(const R&, const S&) const
    {
        FAIL() << typeid(R).name() << " differs from " << typeid(S).name();
    }

    template<typename R>
    void operator()(const R& lhs, const R& rhs) const
    {
        check(lhs, rhs);
    }
};

} // namespace security
} // namespace vanetza

#endif /* CHECK_VISITOR_HPP_YJ7UPXCB */

