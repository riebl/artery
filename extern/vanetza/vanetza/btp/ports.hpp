#ifndef PORTS_HPP_T2IEFSSC
#define PORTS_HPP_T2IEFSSC

#include <vanetza/common/byte_order.hpp>

namespace vanetza
{
namespace btp
{

typedef uint16be_t port_type;

namespace ports
{

static const port_type CAM = host_cast<uint16_t>(2001);
static const port_type DENM = host_cast<uint16_t>(2002);
static const port_type TOPO = host_cast<uint16_t>(2003);
static const port_type SPAT = host_cast<uint16_t>(2004);
static const port_type SAM = host_cast<uint16_t>(2005);

} // namespace ports

} // namespace btp
} // namespace vanetza

#endif /* PORTS_HPP_T2IEFSSC */

