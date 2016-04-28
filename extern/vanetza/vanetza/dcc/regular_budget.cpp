#include "regular_budget.hpp"
#include "state_machine.hpp"

namespace vanetza
{
namespace dcc
{

RegularBudget::RegularBudget(const StateMachine& fsm, const Clock::time_point& clock) :
    m_fsm(fsm), m_clock(clock)
{
}

Clock::duration RegularBudget::delay()
{
    Clock::duration delay = Clock::duration::max();

    if (m_last_tx) {
        const auto last_tx = m_last_tx.get();
        const auto tx_interval = m_fsm.transmission_interval();
        if (last_tx + tx_interval < m_clock) {
            delay = Clock::duration::zero();
        } else {
            delay = last_tx + tx_interval - m_clock;
        }
    } else {
        delay = Clock::duration::zero();
    }

    return delay;
}

void RegularBudget::notify()
{
    m_last_tx = m_clock;
}

} // namespace dcc
} // namespace vanetza
