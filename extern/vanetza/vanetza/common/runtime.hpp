#ifndef RUNTIME_HPP_KHDIEMRN
#define RUNTIME_HPP_KHDIEMRN

#include <vanetza/common/clock.hpp>
#include <boost/bimap/bimap.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <boost/bimap/unordered_multiset_of.hpp>
#include <boost/optional/optional.hpp>
#include <functional>
#include <string>

namespace vanetza
{

/**
 * Runtime provides current time and enables scheduling of tasks for later execution.
 *
 * Time progress has to be triggered explicitly through trigger methods.
 * All calls to Runtime and objects using the same Runtime have to be invoked from same thread!
 **/
class Runtime
{
public:
    using Callback = std::function<void(Clock::time_point)>;

    Runtime() = default;

    /**
     * Create runtime
     * \param init initialization value of internal clock
     */
    Runtime(Clock::time_point init);

    /**
     * Schedule callback for later invocation
     * \param tp invoke callback at this time point
     * \param cb callback
     * \param name optional callback name
     */
    void schedule(Clock::time_point tp, const Callback& cb, const std::string& name = "");

    /**
     * Schedule callback for later invocation
     * \param d duration from now until callback invocation
     * \param cb callback
     * \param name optional callback name
     */
    void schedule(Clock::duration d, const Callback& cb, const std::string& name = "");

    /**
     * Cancel all scheduled invocations of a named callback
     * \param name Name of callback
     */
    void cancel(const std::string& name);

    /**
     * Get time point of next scheduled event
     * \note time point might belong to an expired event, i.e. next() < now()
     * \return time point of next event or time_point::max if none
     */
    Clock::time_point next() const;

    /**
     * Get current time
     * \return current time
     */
    const Clock::time_point& now() const;

    /**
     * Trigger absolute time progress
     *
     * All expired callbacks will be invoked
     * \param tp new time point, has to be greater than now
     */
    void trigger(Clock::time_point tp);

    /**
     * Trigger relative time progress
     *
     * All expired callbacks will be invoked
     * \param d advance time by this duration
     */
    void trigger(Clock::duration d);

    /**
     * Reset runtime
     *
     * Drops all scheduled callbacks and resets internal clock
     * \param tp new time point
     */
    void reset(Clock::time_point tp);

private:
    using queue_bimap = boost::bimaps::bimap<
        boost::bimaps::multiset_of<Clock::time_point>,
        boost::bimaps::unordered_multiset_of<std::string>,
        boost::bimaps::with_info<Callback>
    >;

    void trigger();

    Clock::time_point m_now;
    queue_bimap m_queue;
};

} // namespace vanetza

#endif /* RUNTIME_HPP_KHDIEMRN */

