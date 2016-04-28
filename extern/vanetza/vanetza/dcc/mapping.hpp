#ifndef MAPPING_HPP_MZ2RU7VX
#define MAPPING_HPP_MZ2RU7VX

#include <vanetza/dcc/profile.hpp>
#include <vanetza/net/access_category.hpp>

namespace vanetza
{
namespace dcc
{

/**
 * Map DCC Profile to EDCA access category
 * \param profile DCC Profile ID
 * \return mapped access category
 */
AccessCategory map_profile_onto_ac(Profile);

} // namespace dcc
} // namespace vanetza

#endif /* MAPPING_HPP_MZ2RU7VX */

