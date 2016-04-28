#include "scheduler.hpp"
#include <stdexcept>

namespace vanetza
{
namespace dcc
{

Scheduler::Scheduler(const StateMachine& fsm, const Clock::time_point& clock) :
    m_burst_budget(clock), m_regular_budget(fsm, clock)
{
}

Clock::duration Scheduler::delay(Profile dp)
{
    Clock::duration delay = Clock::duration::max();

    switch (dp) {
        case Profile::DP0:
            delay = m_burst_budget.delay();
            break;
        case Profile::DP1:
        case Profile::DP2:
        case Profile::DP3:
            delay = m_regular_budget.delay();
            break;
        default:
            throw std::invalid_argument("Invalid DCC Profile");
            break;
    };

    return delay;
}

void Scheduler::notify(Profile dp)
{
    switch (dp) {
        case Profile::DP0:
            m_burst_budget.notify();
            break;
        case Profile::DP1:
        case Profile::DP2:
        case Profile::DP3:
            m_regular_budget.notify();
            break;
        default:
            throw std::invalid_argument("Invalid DCC Profile");
            break;
    };
}

} // namespace dcc
} // namespace vanetza
