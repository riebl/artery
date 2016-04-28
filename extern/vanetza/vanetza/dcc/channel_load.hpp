#ifndef CHANNEL_LOAD_HPP_D1JOCNLP
#define CHANNEL_LOAD_HPP_D1JOCNLP

#include <boost/operators.hpp>

namespace vanetza
{
namespace dcc
{

struct ChannelLoad : boost::totally_ordered<ChannelLoad>
{
    ChannelLoad() :
        probes_above(0), probes_total(0) {}
    ChannelLoad(unsigned num, unsigned den) :
        probes_above(num), probes_total(den) {}
    unsigned probes_above;
    unsigned probes_total;

    double fraction() const;
    bool operator<(const ChannelLoad&) const;
};

} // namespace dcc
} // namespace vanetza

#endif /* CHANNEL_LOAD_HPP_D1JOCNLP */

