#ifndef STATE_MACHINE_HPP_YPE958OH
#define STATE_MACHINE_HPP_YPE958OH

#include <vanetza/common/clock.hpp>
#include <vanetza/dcc/channel_load_smoothing.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/optional.hpp>

namespace vanetza
{
namespace dcc
{

// forward declarations
struct ChannelLoad;

// constants
static const Clock::duration NDL_minDccSampling = std::chrono::milliseconds(100);


class State
{
public:
    virtual Clock::duration transmission_interval() const = 0;
    virtual const char* name() const = 0;
    virtual ~State() {}
};

class Relaxed : public State
{
public:
    Clock::duration transmission_interval() const override;
    const char* name() const override;
};

class Active : public State
{
public:
    Active();
    void update(double min, double max);
    Clock::duration transmission_interval() const override;
    const char* name() const override;

private:
    static const std::size_t sc_substates;
    std::size_t m_substate;
};

class Restrictive : public State
{
public:
    Clock::duration transmission_interval() const override;
    const char* name() const override;
};


class StateMachine
{
public:
    StateMachine();
    ~StateMachine();

    /**
     * Notify state machine about current channel load.
     * This method expects to be called in regular intervals
     * of NDL_minDccSampling length.
     */
    void update(const ChannelLoad&);

    /**
     * Get currently allowed maximum message rate depending on state
     * \return messages per second
     */
    double message_rate() const;

    /**
     * Get advised transmission interval between consecutive messages
     * \return message transmission interval
     */
    Clock::duration transmission_interval() const;

    /**
     * Get state machine's active state
     */
    const State& state() const;

    /**
     * Get current channel load (non-smoothed)
     * \return non-smoothed channel load if measurement exists
     */
    boost::optional<double> getChannelLoad() const;

    /**
     * Get current channel load (smoothed)
     * \return smoothed channel load if measurement exists
     */
    boost::optional<double> getSmoothedChannelLoad() const;

private:
    double max_channel_load() const;
    double min_channel_load() const;

    boost::optional<ChannelLoad> m_current_cl;
    Relaxed m_relaxed;
    Active m_active;
    Restrictive m_restrictive;
    State* m_state;
    ChannelLoadSmoothing m_cl_smoothing;
    boost::circular_buffer<double> m_channel_loads;
};

} // namespace dcc
} // namespace vanetza

#endif /* STATE_MACHINE_HPP_YPE958OH */

