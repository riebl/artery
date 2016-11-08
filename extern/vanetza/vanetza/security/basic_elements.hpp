#ifndef BASIC_ELEMENTS_HPP_RALCTYHI
#define BASIC_ELEMENTS_HPP_RALCTYHI

#include <vanetza/common/clock.hpp>
#include <array>
#include <cstdint>

namespace vanetza
{
namespace security
{

using Time64 = uint64_t;
using Time32 = uint32_t;

using HashedId8 = std::array<uint8_t, 8>;
using HashedId3 = std::array<uint8_t, 3>;

HashedId3 truncate(const HashedId8&);

/// Time64WithStandardDeviation specified in TS 103 097 v1.2.1, section 4.2.16
struct Time64WithStandardDeviation
{
    Time64 time64;
    uint8_t log_std_dev;
};

/**
 * Convert time point to time stamp
 * \param tp time point
 * \return time stamp with second accuracy
 */
Time32 convert_time32(const Clock::time_point& tp);

/**
 * Convert time point to time stamp
 * \param tp time point
 * \return time stamp with microsecond accuracy
 */
Time64 convert_time64(const Clock::time_point& tp);

} // namespace security
} // namespace vanetza

#endif /* BASIC_ELEMENTS_HPP_RALCTYHI */
