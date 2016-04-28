#ifndef BURST_BUDGET_HPP_U5GXDCZN
#define BURST_BUDGET_HPP_U5GXDCZN

#include <vanetza/common/clock.hpp>
#include <boost/circular_buffer.hpp>
#include <chrono>
#include <cstddef>

namespace vanetza
{
namespace dcc
{

static const Clock::duration T_Burst = std::chrono::seconds(1);
static const Clock::duration T_BurstPeriod = std::chrono::seconds(10);
constexpr std::size_t N_Burst = 20;

class BurstBudget
{
public:
    BurstBudget(const Clock::time_point&);
    ~BurstBudget();

    /**
     * Get current delay to remain in budget
     * \return shortest delay not exceeding budget
     */
    Clock::duration delay();

    /**
     * Notify budget of consumption
     */
    void notify();

private:
    const Clock::time_point& m_clock;
    boost::circular_buffer<Clock::time_point> m_bursts;
};

} // namespace dcc
} // namespace vanetza

#endif /* BURST_BUDGET_HPP_U5GXDCZN */

