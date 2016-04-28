#include "channel_load.hpp"

namespace vanetza
{
namespace dcc
{

double ChannelLoad::fraction() const
{
    double fraction = 0.0;
    if (probes_total != 0) {
        fraction = probes_above;
        fraction /= probes_total;
    }
    return fraction;
}

bool ChannelLoad::operator<(const ChannelLoad& other) const
{
    return fraction() < other.fraction();
}

} // namespace dcc
} // namespace vanetza

