/*
 * Artery V2X Simulation Framework
 * Copyright 2017 Thiago Vieira, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_GEMV2_MATH_H_RMEYTQN7
#define ARTERY_GEMV2_MATH_H_RMEYTQN7

#include <inet/common/Units.h>
#include <boost/units/pow.hpp>
#include <boost/units/quantity.hpp>

namespace artery
{
namespace gemv2
{

// shortcut for squaring numbers (should be faster than pow(x, 2.0))
constexpr double squared(double x) { return x * x; }

template<typename U, typename V = double>
inet::units::value<V, inet::units::pow<U, 2>> squared(inet::units::value<V, U> x)
{
    return x * x;
}

template<typename U, typename V = double>
typename boost::units::power_typeof_helper<
    boost::units::quantity<V, U>,
    boost::units::static_rational<2>
>::type squared(boost::units::quantity<V, U> x)
{
    return x * x;
}

} // namespace gemv2
} // namespace artery

#endif /* ARTERY_GEMV2_MATH_H_RMEYTQN7 */
