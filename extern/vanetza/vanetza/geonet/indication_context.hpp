#ifndef INDICATION_CONTEXT_HPP_UWOD2BSQ
#define INDICATION_CONTEXT_HPP_UWOD2BSQ

#include <vanetza/common/byte_buffer_source.hpp>
#include <vanetza/geonet/packet.hpp>
#include <vanetza/geonet/pdu.hpp>
#include <vanetza/geonet/variant_pdu.hpp>
#include <boost/iostreams/stream.hpp>
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

    virtual BasicHeader* parse_basic() = 0;
    virtual CommonHeader* parse_common() = 0;
    virtual security::SecuredMessage* parse_secured() = 0;
    virtual boost::optional<HeaderConstRefVariant> parse_extended(HeaderType) = 0;
    virtual const VariantPdu& pdu() const = 0;
    virtual VariantPdu& pdu() = 0;
    virtual UpPacketPtr finish() = 0;
    virtual const LinkLayer& link_layer() const = 0;
    virtual std::size_t payload_length() const = 0;
    virtual ~IndicationContext() = default;
};


namespace detail
{

class IndicationContextDeserializer
{
public:
    IndicationContextDeserializer(const CohesivePacket&);
    std::size_t parse_basic(BasicHeader&);
    std::size_t parse_common(CommonHeader&);
    std::size_t parse_secured(IndicationContext::SecuredMessage&);
    std::size_t parse_extended(HeaderVariant&, HeaderType);
    std::size_t parsed_bytes() const;

private:
    byte_buffer_source m_byte_buffer_source;
    boost::iostreams::stream_buffer<byte_buffer_source> m_stream;
    InputArchive m_archive;
    std::size_t m_read_bytes;
};

class IndicationContextParent : public virtual IndicationContext
{
public:
    IndicationContextParent(const LinkLayer& ll) : m_link_layer(ll) {}
    const LinkLayer& link_layer() const override { return m_link_layer; }
    VariantPdu& pdu() override { return m_pdu; }
    const VariantPdu& pdu() const override { return m_pdu; }

protected:
    LinkLayer m_link_layer;
    VariantPdu m_pdu;
};

class IndicationContextChild : public virtual IndicationContext
{
public:
    IndicationContextChild(IndicationContext& parent) : m_parent(parent) {}
    const LinkLayer& link_layer() const override { return m_parent.link_layer(); }
    VariantPdu& pdu() override { return m_parent.pdu(); }
    const VariantPdu& pdu() const override { return m_parent.pdu(); }

protected:
    IndicationContext& m_parent;
};

} // namespace detail


class IndicationContextDeserialize : public virtual IndicationContext,
    private detail::IndicationContextParent,
    private detail::IndicationContextDeserializer
{
public:
    IndicationContextDeserialize(UpPacketPtr, CohesivePacket&, const LinkLayer&);
    BasicHeader* parse_basic() override;
    CommonHeader* parse_common() override;
    SecuredMessage* parse_secured() override;
    boost::optional<HeaderConstRefVariant> parse_extended(HeaderType) override;
    UpPacketPtr finish() override;
    std::size_t payload_length() const override;

private:
    UpPacketPtr m_packet;
    CohesivePacket& m_cohesive_packet;
};

class IndicationContextCast : public virtual IndicationContext,
    private detail::IndicationContextParent
{
public:
    IndicationContextCast(UpPacketPtr, ChunkPacket&, const LinkLayer&);
    BasicHeader* parse_basic() override;
    CommonHeader* parse_common() override;
    SecuredMessage* parse_secured() override;
    boost::optional<HeaderConstRefVariant> parse_extended(HeaderType) override;
    UpPacketPtr finish() override;
    std::size_t payload_length() const override;

private:
    UpPacketPtr m_packet;
};

class IndicationContextSecuredDeserialize : public virtual IndicationContext,
    private detail::IndicationContextChild,
    private detail::IndicationContextDeserializer
{
public:
    IndicationContextSecuredDeserialize(IndicationContext&, CohesivePacket&);
    BasicHeader* parse_basic() override { return nullptr; }
    CommonHeader* parse_common() override;
    SecuredMessage* parse_secured() override { return nullptr; }
    boost::optional<HeaderConstRefVariant> parse_extended(HeaderType) override;
    UpPacketPtr finish() override;
    std::size_t payload_length() const override;

private:
    CohesivePacket& m_packet;
};

class IndicationContextSecuredCast : public virtual IndicationContext,
    private detail::IndicationContextChild
{
public:
    IndicationContextSecuredCast(IndicationContext&, ChunkPacket&);
    BasicHeader* parse_basic() override { return nullptr; }
    CommonHeader* parse_common() override;
    SecuredMessage* parse_secured() override { return nullptr; }
    boost::optional<HeaderConstRefVariant> parse_extended(HeaderType) override;
    UpPacketPtr finish() override;
    std::size_t payload_length() const override;

private:
    UpPacketPtr m_packet;
};

} // namespace geonet
} // namespace vanetza

#endif /* INDICATION_CONTEXT_HPP_UWOD2BSQ */

