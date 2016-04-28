#ifndef CHECK_LIST_HPP_1ONCXSED
#define CHECK_LIST_HPP_1ONCXSED

#include <gtest/gtest.h>
#include <boost/format.hpp>
#include <list>
#include <typeinfo>

namespace vanetza
{
namespace security
{

template<typename T>
void check(std::list<T> expected, std::list<T> actual, const char* type = typeid(T).name())
{
    SCOPED_TRACE(boost::format("list<%1%>") % type);
    ASSERT_EQ(expected.size(), actual.size());
    std::size_t i = 0;
    while (!expected.empty() && !actual.empty()) {
        SCOPED_TRACE(boost::format("%1% #%2%") % type % i);
        check(expected.front(), actual.front());
        expected.pop_front();
        actual.pop_front();
        ++i;
    }
}

} // namespace security
} // namespace vanetza


#endif /* CHECK_LIST_HPP_1ONCXSED */

