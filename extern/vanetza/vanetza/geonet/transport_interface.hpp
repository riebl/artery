#ifndef TRANSPORT_INTERFACE_HPP_MIGTMAYG
#define TRANSPORT_INTERFACE_HPP_MIGTMAYG

#include <vanetza/geonet/packet.hpp>
#include <memory>

namespace vanetza
{

namespace geonet
{

struct DataIndication;

class TransportInterface
{
public:
    virtual void indicate(const DataIndication&, std::unique_ptr<UpPacket>) = 0;
    virtual ~TransportInterface() {}
};

} // namespace geonet
} // namespace vanetza

#endif /* TRANSPORT_INTERFACE_HPP_MIGTMAYG */

