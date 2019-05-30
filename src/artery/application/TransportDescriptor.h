#ifndef ARTERY_TRANSPORTDESCRIPTOR_H_JIYPGNCY
#define ARTERY_TRANSPORTDESCRIPTOR_H_JIYPGNCY

#include "artery/utility/Channel.h"
#include <cstdint>
#include <tuple>

namespace artery
{

using PortNumber = std::uint16_t;
using TransportDescriptor = std::tuple<ChannelNumber, PortNumber>;

inline ChannelNumber getChannel(const TransportDescriptor& td)
{
    return std::get<0>(td);
}

inline PortNumber getPort(const TransportDescriptor& td)
{
    return std::get<1>(td);
}

} // namespace artery

#endif /* ARTERY_TRANSPORTDESCRIPTOR_H_JIYPGNCY */

