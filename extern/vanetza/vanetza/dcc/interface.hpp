#ifndef INTERFACE_HPP_4SUUTA6X
#define INTERFACE_HPP_4SUUTA6X

#include <memory>

namespace vanetza
{

// forward declarations
class ChunkPacket;

namespace dcc
{

// forward declarations
struct DataRequest;

class RequestInterface
{
public:
    virtual void request(const DataRequest&, std::unique_ptr<ChunkPacket>) = 0;
};

} // namespace dcc
} // namespace vanetza

#endif /* INTERFACE_HPP_4SUUTA6X */

