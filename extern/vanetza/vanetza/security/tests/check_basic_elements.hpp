#ifndef CHECK_BASIC_ELEMENTS_HPP_XLHVYJ0S
#define CHECK_BASIC_ELEMENTS_HPP_XLHVYJ0S

#include <gtest/gtest.h>
#include <vanetza/security/basic_elements.hpp>
#include <vanetza/security/int_x.hpp>
#include <boost/format.hpp>
#include <type_traits>

namespace vanetza
{
namespace security
{

template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
void check(const T& expected, const T& actual)
{
    EXPECT_EQ(expected, actual);
}

template<typename T, size_t N>
void check(const std::array<T, N>& expected, const std::array<T, N>& actual)
{
    SCOPED_TRACE("array<T, N>");
    for (unsigned i = 0; i < N; ++i) {
        SCOPED_TRACE(boost::format("element index #%1%") % i);
        check(expected[i], actual[i]);
    }
}

void check(const Time64WithStandardDeviation&, const Time64WithStandardDeviation&);
void check(const IntX&, const IntX&);

// explicit template instantiations
template void check<uint8_t, 3>(const HashedId3&, const HashedId3&);
template void check<uint8_t, 8>(const HashedId8&, const HashedId8&);
template void check<Time64>(const Time64&, const Time64&);
template void check<Time32>(const Time32&, const Time32&);

} // namespace security
} // namespace vanetza

#endif /* CHECK_BASIC_ELEMENTS_HPP_XLHVYJ0S */
