#include "burst_budget.hpp"

namespace vanetza
{
namespace dcc
{

BurstBudget::BurstBudget(const Clock::time_point& clock) :
    m_clock(clock), m_bursts(N_Burst)
{
}

BurstBudget::~BurstBudget()
{
}

Clock::duration BurstBudget::delay()
{
    Clock::duration delay = Clock::duration::max();

    if (m_bursts.empty()) {
        delay = Clock::duration::zero();
    } else if (m_bursts.front() + T_Burst > m_clock && !m_bursts.full()) {
        delay = Clock::duration::zero();
    } else if (m_bursts.front() + T_BurstPeriod < m_clock) {
        m_bursts.clear();
        delay = Clock::duration::zero();
    } else {
        delay = m_bursts.front() + T_BurstPeriod - m_clock;
    }

    return delay;
}

void BurstBudget::notify()
{
    m_bursts.push_back(m_clock);
}

} // namespace dcc
} // namespace vanetza
