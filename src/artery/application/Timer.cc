#include "artery/application/Timer.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <omnetpp/csimulation.h>
#include <chrono>
#include <ratio>

using namespace omnetpp;

void Timer::setTimebase(const std::string& datetime)
{
        auto tb = boost::posix_time::time_from_string(datetime);
        mTimebase = vanetza::Clock::at(tb);
}

vanetza::Clock::time_point Timer::getCurrentTime() const
{
        auto now = simTime();
	return getTimeFor(now);
}

vanetza::Clock::time_point Timer::getTimeFor(SimTime simtime) const
{
	static_assert(std::ratio_less_equal<vanetza::Clock::period, std::micro>::value,
		"At least microsecond precision is required of Vanetza clock");
	return mTimebase + std::chrono::microseconds(simtime.inUnit(SIMTIME_US));
}

uint64_t countTaiMilliseconds(vanetza::Clock::time_point tp)
{
    using namespace std::chrono;
    const auto d = tp.time_since_epoch();
    const auto ms = duration_cast<milliseconds>(d);
    return ms.count();
}
