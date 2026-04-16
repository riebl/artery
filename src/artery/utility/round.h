#ifndef ARTERY_UTILITY_ROUND_H_
#define ARTERY_UTILITY_ROUND_H_

#include <boost/units/cmath.hpp>

#include <cmath>

namespace artery
{

template <typename T, typename U>
long round(const boost::units::quantity<T>& q, const U& u)
{
    boost::units::quantity<U> v{q};
    return std::round(v.value());
}

}  // namespace artery

#endif /* ARTERY_UTILITY_ROUND_H_ */