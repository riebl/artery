#ifndef CHANNEL_H
#define CHANNEL_H

#include <cstdint>

namespace artery
{

using Channel = int;

namespace channel
{

/**
 * Channel assigned for ETSI ITS
 * \see TS 102 965 V1.3.1 Annex A
 */
constexpr Channel SCH0 = 180;
constexpr Channel CCH = SCH0;

constexpr Channel SCH1 = 176;
constexpr Channel SCH2 = 178;

constexpr Channel SCH3 = 174;
constexpr Channel SCH4 = 172;

constexpr Channel SCH5 = 182;
constexpr Channel SCH6 = 184;

} // namespace channel
} // namespace artery

#endif /* CHANNEL_H */

