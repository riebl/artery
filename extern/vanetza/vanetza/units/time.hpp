#ifndef TIME_HPP_GFC1AX6E
#define TIME_HPP_GFC1AX6E

#include <vanetza/common/clock.hpp>
#include <boost/units/quantity.hpp>
#include <boost/units/systems/si/time.hpp>

namespace vanetza
{
namespace units
{

namespace si = boost::units::si;
typedef boost::units::quantity<boost::units::si::time> Duration;

/**
 * Convert a Boost.Units duration to a std::chrono::duration
 * \tparam U unit type
 * \tparam T value type
 * \param duration Boost.Units based duration
 * \return std::chrono::duration representative of duration
 */
template<typename U, typename T>
Clock::duration clock_cast(const boost::units::quantity<U, T>& duration)
{
    using chrono_type = std::chrono::duration<Duration::value_type>;
    const Duration quantity { duration };
    const chrono_type chrono_duration { quantity / si::seconds };
    return std::chrono::duration_cast<Clock::duration>(chrono_duration);
}

/**
 * Convert std::chrono::duration to a Boost.Units quantity
 * \tparam T type representing ticks
 * \tparam P tick period type
 * \param duration std::chrono::duration based duration
 * \return Boost.Units quantity representing duration
 */
template<class T, class P>
Duration clock_cast(const std::chrono::duration<T, P>& duration)
{
    using chrono_type = std::chrono::duration<Duration::value_type>;
    const auto value = std::chrono::duration_cast<chrono_type>(duration);
    return value.count() * si::seconds;
}

} // namespace units
} // namespace vanetza

#endif /* TIME_HPP_GFC1AX6E */

