#ifndef ARTERY_SIMTIME_CAST_H_EYKBHFUV
#define ARTERY_SIMTIME_CAST_H_EYKBHFUV

#include <omnetpp/simtime.h>
#include <chrono>
#include <ratio>

namespace artery
{

template<typename PERIOD>
constexpr omnetpp::SimTimeUnit simtime_unit();

template<> constexpr omnetpp::SimTimeUnit simtime_unit<std::ratio<1>>() { return omnetpp::SIMTIME_S; }
template<> constexpr omnetpp::SimTimeUnit simtime_unit<std::milli>() { return omnetpp::SIMTIME_MS; }
template<> constexpr omnetpp::SimTimeUnit simtime_unit<std::micro>() { return omnetpp::SIMTIME_US; }
template<> constexpr omnetpp::SimTimeUnit simtime_unit<std::nano>() { return omnetpp::SIMTIME_NS; }
template<> constexpr omnetpp::SimTimeUnit simtime_unit<std::pico>() { return omnetpp::SIMTIME_PS; }
template<> constexpr omnetpp::SimTimeUnit simtime_unit<std::femto>() { return omnetpp::SIMTIME_FS; }

/**
 * Create a SimTime object from an arbitrary std::chrono::duration
 * \param d a std::chrono::duration
 * \return SimTime representing d
 */
template<typename Rep, typename Period>
omnetpp::SimTime simtime_cast(const std::chrono::duration<Rep, Period>& d)
{
    return omnetpp::SimTime { d.count(), simtime_unit<Period>() };
}

} // namespace artery

#endif /* ARTERY_SIMTIME_CAST_H_EYKBHFUV */

