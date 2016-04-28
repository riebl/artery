#ifndef CHECK_REGION_HPP_UFSV2RZ5
#define CHECK_REGION_HPP_UFSV2RZ5

#include <vanetza/security/region.hpp>

namespace vanetza
{
namespace security
{

/** \brief check two TwoDLocations
 * \param expected the expected value
 * \param actual the actual value
 */
void check(const TwoDLocation& expected, const TwoDLocation& actual);

/** \brief check two ThreeDLocations
 * \param expected the expected value
 * \param actual the actual value
 */
void check(const ThreeDLocation&, const ThreeDLocation&);

/** \brief check two CircularRegions
 * \param expected the expected value
 * \param actual the actual value
 */
void check(const CircularRegion& expected, const CircularRegion& actual);

/** \brief check two RectangularRegions
 * \param expected the expected value
 * \param actual the actual value
 */
void check(const RectangularRegion& expected, const RectangularRegion& actual);

/** \brief check two std::list<RectangularRegion>
 * \param expected the expected list
 * \param actual the actual value
 */
void check(std::list<RectangularRegion> expected, std::list<RectangularRegion> actual);

/** \brief check two PolygonalRegions
 * \param expected the expected value
 * \param actual the actual value
 */
void check(PolygonalRegion expected, PolygonalRegion actual);

/** \brief check two IdentifiedRegions
 * \param expected the expected value
 * \param actual the actual value
 */
void check(const IdentifiedRegion& expected, const IdentifiedRegion& actual);

/** \brief check two GeographicRegion
 * \param expected the expected value
 * \param actual the actual value
 */
void check(const GeographicRegion& expected, const GeographicRegion& actual);

} // namespace security
} // namespace vanetza

#endif /* CHECK_REGION_HPP_UFSV2RZ5 */
