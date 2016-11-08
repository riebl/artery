#include <vanetza/common/clock.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace vanetza
{

const boost::posix_time::ptime& Clock::epoch()
{
    static const boost::posix_time::ptime epoch {
        boost::gregorian::date(2004, 1, 1),
        boost::posix_time::milliseconds(0)
    };
    return epoch;
}

Clock::time_point Clock::at(const boost::posix_time::ptime& t)
{
    auto delta = (t - epoch()).total_microseconds();
    Clock::time_point tp { std::chrono::microseconds(delta) };
    return tp;
}

Clock::date_time Clock::at(const Clock::time_point& t)
{
    std::chrono::microseconds delta = t.time_since_epoch();
    return Clock::epoch() + boost::posix_time::microseconds(delta.count());
}

Clock::time_point Clock::at(const std::string& at)
{
    return Clock::at(boost::posix_time::time_from_string(at));
}

} // namespace vanetza
