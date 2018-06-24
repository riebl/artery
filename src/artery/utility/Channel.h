#ifndef CHANNEL_H
#define CHANNEL_H

#include <functional>
#include <string>

namespace artery
{

using ChannelNumber = unsigned;

/**
 * Parse channel number from a string.
 * This functions can parse integers ("180") and symbolic channel names ("CCH").
 *
 * \param str string with channel number
 * \return parsed channel number or 0
 */
ChannelNumber parseChannelNumber(const std::string& str);

/**
 * Get rank of a channel number.
 * This is useful for sorting channels by CCH, SCH1, SCH2 etc.
 *
 * \param ch channel number
 * \return rank for given channel number
 */
unsigned getChannelRank(ChannelNumber ch);

/**
 * Comparison function object to sort by channel rank
 *
 * \param a lhs channel number
 * \param b rhs channel number
 * \return true if a is "more important" than b (has lower rank)
 */
static auto ChannelRankCompare = [](ChannelNumber a, ChannelNumber b) -> bool {
    return getChannelRank(a) < getChannelRank(b);
};

namespace channel
{

/**
 * Channel assigned for ETSI ITS
 * \see TS 102 965 V1.3.1 Annex A
 */
constexpr ChannelNumber CCH = 180;
constexpr ChannelNumber SCH0 = CCH;

constexpr ChannelNumber SCH1 = 176;
constexpr ChannelNumber SCH2 = 178;

constexpr ChannelNumber SCH3 = 174;
constexpr ChannelNumber SCH4 = 172;

constexpr ChannelNumber SCH5 = 182;
constexpr ChannelNumber SCH6 = 184;

} // namespace channel
} // namespace artery

#endif /* CHANNEL_H */

