#ifndef CHECK_ECC_POINT_HPP_UQH2R8WK
#define CHECK_ECC_POINT_HPP_UQH2R8WK

#include <gtest/gtest.h>
#include <vanetza/security/ecc_point.hpp>
#include <vanetza/security/tests/check_visitor.hpp>
#include <boost/variant/apply_visitor.hpp>

namespace vanetza
{
namespace security
{

/**
 * \brief check two X_Coordinate_Only
 * \param expected the expected value
 * \param actual the actual value
 */
inline void check(const X_Coordinate_Only& expected, const X_Coordinate_Only& actual)
{
    EXPECT_EQ(expected.x, actual.x);
}

/**
 * \brief check two Compressed_Lsb_Y_0
 * \param expected the expected value
 * \param actual the actual value
 */
inline void check(const Compressed_Lsb_Y_0& expected, const Compressed_Lsb_Y_0& actual)
{
    EXPECT_EQ(expected.x, actual.x);
}

/**
 * \brief check two Compressed_Lsb_Y_1
 * \param expected the expected value
 * \param actual the actual value
 */
inline void check(const Compressed_Lsb_Y_1& expected, const Compressed_Lsb_Y_1& actual)
{
    EXPECT_EQ(expected.x, actual.x);
}

/**
 * \brief check two Uncompressed
 * \param expected the expected value
 * \param actual the actual value
 */
inline void check(const Uncompressed& expected, const Uncompressed& actual)
{
    EXPECT_EQ(expected.x, actual.x);
    EXPECT_EQ(expected.y, actual.y);
}

/**
 * \brief check two EccPoints
 * \param expected the expected value
 * \param actual the actual value
 */
inline void check(const EccPoint& expected, const EccPoint& actual)
{
    ASSERT_EQ(get_type(expected), get_type(actual));
    boost::apply_visitor(check_visitor<EccPoint>(), expected, actual);
}

} // namespace security
} // namespace vanetza

#endif /* CHECK_ECC_POINT_HPP_UQH2R8WK */
