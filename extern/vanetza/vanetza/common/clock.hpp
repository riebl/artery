#ifndef CLOCK_HPP_2FCBLXSJ
#define CLOCK_HPP_2FCBLXSJ

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <chrono>
#include <cstdint>
#include <ratio>

namespace vanetza
{

/**
 * A Clock similar to std::chrono with epoch at 2004-01-01 at midnight
 */
class Clock
{
public:
    typedef uint64_t rep;
    typedef std::ratio<1, 1000 * 1000> period;
    typedef std::chrono::duration<rep, period> duration;
    typedef std::chrono::time_point<Clock> time_point;
    typedef boost::posix_time::ptime date_time;

    static constexpr bool is_steady() { return true; }
    static time_point at(const date_time&);
    static const date_time& epoch();
};

} // namespace vanetza

#endif /* CLOCK_HPP_2FCBLXSJ */

