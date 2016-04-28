#include "mapping.hpp"
#include <stdexcept>

namespace vanetza
{
namespace dcc
{

AccessCategory map_profile_onto_ac(Profile dp_id)
{
    AccessCategory ac = AccessCategory::BE;

    switch (dp_id)
    {
        case Profile::DP0:
            ac = AccessCategory::VO;
            break;
        case Profile::DP1:
            ac = AccessCategory::VI;
            break;
        case Profile::DP2:
            ac = AccessCategory::BE;
            break;
        case Profile::DP3:
            ac = AccessCategory::BK;
            break;
        default:
            throw std::invalid_argument("Invalid DCC Profile ID");
            break;
    }

    return ac;
}

} // namespace dcc
} // namespace vanetza
