#ifndef DESTINATION_VARIANT_HPP_Y4TEVUXO
#define DESTINATION_VARIANT_HPP_Y4TEVUXO

#include <vanetza/geonet/areas.hpp>
#include <vanetza/geonet/address.hpp>
#include <boost/variant.hpp>

namespace vanetza
{
namespace geonet
{

typedef boost::variant<Address, Area, std::nullptr_t> DestinationVariant;

} // namespace geonet
} // namespace vanetza

#endif /* DESTINATION_VARIANT_HPP_Y4TEVUXO */

