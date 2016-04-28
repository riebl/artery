#ifndef CAM_FUNCTIONS_HPP_PUFKBEM8
#define CAM_FUNCTIONS_HPP_PUFKBEM8

// forward declaration of asn1c generated struct
struct BasicVehicleContainerLowFrequency;

namespace vanetza
{
namespace facilities
{

class PathHistory;

/**
 * Copy PathHistory into BasicVehicleContainerLowFrequency's pathHistory element
 * \param Facilities' path history object (source)
 * \param ASN.1 CAM container (destination)
 */
void copy(const PathHistory&, BasicVehicleContainerLowFrequency&);

} // namespace facilities
} // namespace vanetza

#endif /* CAM_FUNCTIONS_HPP_PUFKBEM8 */
