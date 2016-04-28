#ifndef INTERFACE_HPP_EUPJ90MD
#define INTERFACE_HPP_EUPJ90MD

#include <vanetza/net/chunk_packet.hpp>
#include <memory>

namespace vanetza
{
namespace access
{

struct DataRequest;

class Interface
{
public:
    virtual void request(const DataRequest&, std::unique_ptr<ChunkPacket>) = 0;
    virtual ~Interface() {}
};

} // namespace access
} // namespace vanetza

#endif /* INTERFACE_HPP_EUPJ90MD */

