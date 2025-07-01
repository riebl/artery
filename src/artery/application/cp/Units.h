/*
 * Artery V2X Simulation Framework
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_CP_UNITS_H_
#define ARTERY_CP_UNITS_H_

/**
 * @file Units.h
 * @brief Units and helper functions for CPM's
 */

#include <boost/units/quantity.hpp>
#include <boost/units/systems/si/prefixes.hpp>
#include <vanetza/units/acceleration.hpp>
#include <vanetza/units/angle.hpp>
#include <vanetza/units/length.hpp>
#include <vanetza/units/time.hpp>
#include <vanetza/units/velocity.hpp>

#include <cmath>


namespace artery
{
namespace cp
{
namespace units
{

constexpr auto decidegree = vanetza::units::degree * boost::units::si::deci;
constexpr auto microdegree = vanetza::units::degree * boost::units::si::micro;
constexpr auto microdecidegree = vanetza::units::degree * boost::units::si::micro * boost::units::si::deci;
constexpr auto degree_per_second = vanetza::units::degree / vanetza::units::si::second;
constexpr auto centidegree_per_second = vanetza::units::degree * boost::units::si::centi / vanetza::units::si::second;
constexpr auto centimeter = vanetza::units::si::meter * boost::units::si::centi;
constexpr auto centimeter_per_second = vanetza::units::si::meter_per_second * boost::units::si::centi;
constexpr auto decimeter_per_second_squared = vanetza::units::si::meter_per_second_squared * boost::units::si::deci;

}  // namespace units

/**
 * @brief Round value from source unit into target unit
 *
 * @tparam T Source unit type
 * @tparam U Target unit type
 * @param q Source value
 * @param u Target unit
 * @return Target value as rounded raw value
 */
template <typename T, typename U>
long roundToUnit(const boost::units::quantity<T>& q, const U& u)
{
    boost::units::quantity<U> v(q);

    return std::round(v.value());
}

/**
 * @brief Convert value from source unit into target unit
 *
 * @tparam T Source unit type
 * @tparam U Target unit type
 * @param q Source value
 * @param u Target unit
 * @return Target value as raw value
 */
template <typename T, typename U>
double convertToUnit(const boost::units::quantity<T>& q, const U& u)
{
    boost::units::quantity<U> v(q);

    return v.value();
}

}  // namespace cp
}  // namespace artery

#endif /* ARTERY_CP_UNITS_H_ */
