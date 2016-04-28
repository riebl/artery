#ifndef DATA_INTERFACE_HPP_HKI9B5SC
#define DATA_INTERFACE_HPP_HKI9B5SC

#include <vanetza/btp/data_indication.hpp>
#include <vanetza/btp/data_request.hpp>
#include <vanetza/common/byte_order.hpp>
#include <vanetza/net/packet.hpp>
#include <memory>

namespace vanetza
{
namespace btp
{

class IndicationInterface
{
public:
    virtual void indicate(const DataIndication&, std::unique_ptr<UpPacket>) = 0;
    virtual ~IndicationInterface() {};
};

class RequestInterface
{
public:
    virtual void request(const DataRequestB&, std::unique_ptr<DownPacket>) = 0;
    virtual ~RequestInterface() {};
};

} // namespace btp
} // namespace vanetza

#endif /* DATA_INTERFACE_HPP_HKI9B5SC */

