#ifndef ARTERY_SAMPLEBUFFERALGORITHM_H_HIUNJPAK
#define ARTERY_SAMPLEBUFFERALGORITHM_H_HIUNJPAK

#include <boost/units/operators.hpp>
#include <boost/units/quantity.hpp>
#include <boost/units/systems/si/time.hpp>
#include <cassert>
#include <numeric>
#include <type_traits>

template<typename T>
boost::units::quantity<boost::units::si::time> duration(const Sample<T>& from, const Sample<T>& to)
{
    const omnetpp::SimTime& t_from = from.timestamp;
    const omnetpp::SimTime& t_to = to.timestamp;
    return (t_to - t_from).dbl() * boost::units::si::seconds;
}

template<typename T>
auto differentiate(const Sample<T>& from, const Sample<T>& to) ->
boost::units::quantity<
    typename boost::units::divide_typeof_helper<
        typename T::unit_type,
        typename boost::units::si::time
    >::type
>
{
    const auto d = duration(from, to);
    assert(d.value() != 0.0);
    return (to.value - from.value) / d;
}

template<
    typename RANGE,
    typename SAMPLE = typename boost::range_value<RANGE>::type,
    typename VALUE = typename SAMPLE::value_type,
    typename std::enable_if<std::is_same<Sample<VALUE>, SAMPLE>::value>::type* = nullptr>
VALUE average(RANGE r)
{
    VALUE avg;
    if (r.begin() != r.end()) {
        avg = std::accumulate(r.begin(), r.end(), avg,
                [](const VALUE& sum, const SAMPLE& sample) { return sum + sample.value; });
        avg /= r.size();
    }
    return avg;
}

#endif /* ARTERY_SAMPLEBUFFERALGORITHM_H_HIUNJPAK */

