#include "channel_load_smoothing.hpp"

namespace vanetza
{
namespace dcc
{

void ChannelLoadSmoothing::update(ChannelLoad now)
{
    const double alpha = 0.5;
    const double beta = 0.5;

    m_smoothed = alpha * m_smoothed + beta * now.fraction();
}

} // namespace dcc
} // namespace vanetza
