#ifndef INDICATION_CONTEXT_HPP_UWOD2BSQ
#define INDICATION_CONTEXT_HPP_UWOD2BSQ

#include <vanetza/geonet/data_indication.hpp>
#include <vanetza/geonet/packet.hpp>
#include <vanetza/geonet/parser.hpp>
#include <vanetza/geonet/pdu.hpp>
#include <vanetza/geonet/variant_pdu.hpp>
#include <boost/optional/optional.hpp>

namespace vanetza
{
namespace geonet
{

/**
 * IndicationContext is used for parsing incoming packets.
 * For each indication (packet reception) a new context should be created.
 */
class IndicationContext
{
public:
    using UpPacketPtr = std::unique_ptr<UpPacket>;
    using SecuredMessage = security::SecuredMessageV2;

    struct LinkLayer
    {
        MacAddress sender;
        MacAddress destination;
    };

    // parser commands
    virtual const BasicHeader* parse_basic() = 0;
    virtual const CommonHeader* parse_common() = 0;
    virtual const SecuredMessage* parse_secured() = 0;
    virtual boost::optional<HeaderConstRefVariant> parse_extended(HeaderType) = 0;

    // access to data structures related to indication
    virtual const VariantPdu& pdu() const = 0;
    virtual VariantPdu& pdu() = 0;
    virtual const LinkLayer& link_layer() const = 0;
    virtual DataIndication& service_primitive() = 0;

    /**
     * Finish usage of IndicationContext and release owned packet
     * \return owned packet
     */
    virtual UpPacketPtr finish() = 0;

    virtual ~IndicationContext() = default;
};


/**
 * IndicationContextBasic represents the first phase of packet reception,
 * i.e. it is the context for unsecured headers
 */
class IndicationContextBasic : public IndicationContext
{
public:
    IndicationContextBasic(const LinkLayer& ll) : m_link_layer(ll) {}
    const LinkLayer& link_layer() const override { return m_link_layer; }
    DataIndication& service_primitive() override { return m_service_primitive; }
    VariantPdu& pdu() override { return m_pdu; }
    const VariantPdu& pdu() const override { return m_pdu; }

protected:
    LinkLayer m_link_layer;
    DataIndication m_service_primitive;
    VariantPdu m_pdu;
};

class IndicationContextDeserialize : public IndicationContextBasic
{
public:
    IndicationContextDeserialize(UpPacketPtr, CohesivePacket&, const LinkLayer&);
    const BasicHeader* parse_basic() override;
    const CommonHeader* parse_common() override;
    const SecuredMessage* parse_secured() override;
    boost::optional<HeaderConstRefVariant> parse_extended(HeaderType) override;
    UpPacketPtr finish() override;

private:
    UpPacketPtr m_packet;
    CohesivePacket& m_cohesive_packet;
    Parser m_parser;
};

class IndicationContextCast : public IndicationContextBasic
{
public:
    IndicationContextCast(UpPacketPtr, ChunkPacket&, const LinkLayer&);
    const BasicHeader* parse_basic() override;
    const CommonHeader* parse_common() override;
    const SecuredMessage* parse_secured() override;
    boost::optional<HeaderConstRefVariant> parse_extended(HeaderType) override;
    UpPacketPtr finish() override;

private:
    UpPacketPtr m_packet;
};


/**
 * IndicationContextSecured is used for the (optional) second phase of packet reception,
 * i.e. handling the payload contained in a secured message's payload
 */
class IndicationContextSecured : public IndicationContext
{
public:
    IndicationContextSecured(IndicationContextBasic& parent) : m_parent(parent) {}
    const LinkLayer& link_layer() const override { return m_parent.link_layer(); }
    DataIndication& service_primitive() override { return m_parent.service_primitive(); }
    VariantPdu& pdu() override { return m_parent.pdu(); }
    const VariantPdu& pdu() const override { return m_parent.pdu(); }
    const BasicHeader* parse_basic() override { return nullptr; }
    const SecuredMessage* parse_secured() override { return nullptr; }

protected:
    IndicationContextBasic& m_parent;
};

class IndicationContextSecuredDeserialize : public IndicationContextSecured
{
public:
    IndicationContextSecuredDeserialize(IndicationContextBasic&, CohesivePacket&);
    const CommonHeader* parse_common() override;
    boost::optional<HeaderConstRefVariant> parse_extended(HeaderType) override;
    UpPacketPtr finish() override;

private:
    CohesivePacket& m_packet;
    Parser m_parser;
};

class IndicationContextSecuredCast : public IndicationContextSecured
{
public:
    IndicationContextSecuredCast(IndicationContextBasic&, ChunkPacket&);
    const CommonHeader* parse_common() override;
    boost::optional<HeaderConstRefVariant> parse_extended(HeaderType) override;
    UpPacketPtr finish() override;

private:
    UpPacketPtr m_packet;
};

} // namespace geonet
} // namespace vanetza

#endif /* INDICATION_CONTEXT_HPP_UWOD2BSQ */

