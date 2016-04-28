#ifndef PROFILE_HPP_K04O16SX
#define PROFILE_HPP_K04O16SX

#include <cstdint>

namespace vanetza
{
namespace security
{

/** \brief Profile types specified in TS 103 097 v1.2.1 in 7.1, 7.2 and 7.3
 */
enum class Profile : uint8_t
{
    Generic = 0,
    CAM = 1,
    DENM = 2,
};

} // namespace security
} // namespace vanetza

#endif /* PROFILE_HPP_K04O16SX */
