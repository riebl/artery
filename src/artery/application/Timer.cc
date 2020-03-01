#include "artery/application/Timer.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <omnetpp/csimulation.h>
#include <chrono>
#include <ratio>

using namespace omnetpp;

namespace artery
{

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

omnetpp::SimTime Timer::getTimeFor(vanetza::Clock::time_point tai) const
{
        std::chrono::microseconds diff = tai - mTimebase;
        SimTime sim = diff.count() < 0 ? SimTime::ZERO : SimTime { diff.count(), SIMTIME_US };
        return sim;
}

vanetza::Clock::time_point Timer::reconstructMilliseconds(uint16_t ms16) const
{
    const uint64_t tai_now = countTaiMilliseconds(getCurrentTime());
    const uint64_t tai_now_msb = tai_now & ~0xffff;
    uint64_t tai_rec = tai_now_msb | ms16;

    if (tai_now > tai_rec + 0x7fff) {
        tai_rec += 0x10000;
        assert(tai_rec - tai_now <= 0x8000);
    } else if (tai_now + 0x7fff < tai_rec) {
        tai_rec -= 0x10000;
        assert(tai_now - tai_rec <= 0x8000);
    }

    return vanetza::Clock::time_point { std::chrono::milliseconds(tai_rec) };
}

uint64_t countTaiMilliseconds(vanetza::Clock::time_point tp)
{
    using namespace std::chrono;
    const auto d = tp.time_since_epoch();
    const auto ms = duration_cast<milliseconds>(d);
    return ms.count();
}

} // namespace artery
