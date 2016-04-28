#ifndef FAKE_INTERFACES_HPP
#define FAKE_INTERFACES_HPP

#include <vanetza/dcc/data_request.hpp>
#include <vanetza/dcc/interface.hpp>
#include <vanetza/geonet/transport_interface.hpp>
#include <vanetza/geonet/data_indication.hpp>
#include <vanetza/geonet/data_confirm.hpp>

using namespace vanetza;

class FakeRequestInterface : public dcc::RequestInterface
{
public:
    FakeRequestInterface() : m_requests(0) {}

    void request(const dcc::DataRequest& req, std::unique_ptr<ChunkPacket> packet) override
    {
        ++m_requests;
        m_last_request = req;
        m_last_packet = std::move(packet);
    }

    unsigned m_requests;
    dcc::DataRequest m_last_request;
    std::unique_ptr<ChunkPacket> m_last_packet;
};

class FakeTransportInterface : public geonet::TransportInterface
{
public:
    FakeTransportInterface() : m_indications(0) {}

    void indicate(const geonet::DataIndication& ind, std::unique_ptr<geonet::UpPacket> packet) override
    {
        ++m_indications;
        m_last_indication = ind;
        m_last_packet = std::move(packet);
    }

    unsigned m_indications;
    boost::optional<geonet::DataIndication> m_last_indication;
    std::unique_ptr<geonet::UpPacket> m_last_packet;
};

#endif // FAKE_INTERFACES_HPP
