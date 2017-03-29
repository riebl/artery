#ifndef ARTERY_TIMER_H_EQBBXFD0
#define ARTERY_TIMER_H_EQBBXFD0

#include <omnetpp/simtime.h>
#include <vanetza/common/clock.hpp>
#include <cstdint>
#include <string>

/**
 * Timer represents the timing entity of Facilities Position and Time Management (POTI).
 *
 * OMNeT++ simulation time is linked to TAI (international atomic time) through a given time offset,
 * i.e. this "time base" is the TAI time point corresponding to simulation time zero.
 */
class Timer
{
	public:
		/**
		 * Set time base corresponding to simtime zero
		 * \param string with format YYYY-MM-DD HH:mm:ss
		 */
		void setTimebase(const std::string& datetime);

		/**
		 * Get TAI time of current simulation time
		 * \return current TAI time point
		 */
		vanetza::Clock::time_point getCurrentTime() const;

		/**
		 * Get TAI time point based on given simulation time
		 * \param simtime simulation time point
		 * \return corresponding TAI clock time point
		 */
		vanetza::Clock::time_point getTimeFor(omnetpp::SimTime simtime) const;

		/**
		 * Get simulation time for TAI time point
		 * \param tai time point
		 * \return corresponding simulation time
		 */
		omnetpp::SimTime getTimeFor(vanetza::Clock::time_point) const;

		/**
		 * Reconstruct time point from a MOD 2^16 time stamp with millisecond granuality
		 * \param ms time stamp
		 * \return reconstructed time point
		 */
		vanetza::Clock::time_point reconstructMilliseconds(uint16_t ms) const;

	private:
		vanetza::Clock::time_point mTimebase;
};

uint64_t countTaiMilliseconds(vanetza::Clock::time_point);

#endif /* ARTERY_TIMER_H_EQBBXFD0 */
