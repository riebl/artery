#ifndef CHANNEL_LOAD_SMOOTHING_HPP_TIJ4W5U3
#define CHANNEL_LOAD_SMOOTHING_HPP_TIJ4W5U3

#include <vanetza/dcc/channel_load.hpp>

namespace vanetza
{
namespace dcc
{

class ChannelLoadSmoothing
{
public:
    ChannelLoadSmoothing() : m_smoothed(0.0) {}
    void update(ChannelLoad now);
    double channel_load() const { return m_smoothed; }

private:
    double m_smoothed;
};

} // namespace dcc
} // namespace vanetza

#endif /* CHANNEL_LOAD_SMOOTHING_HPP_TIJ4W5U3 */

