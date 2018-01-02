#include <vanetza/geonet/variant_pdu.hpp>
#include <boost/variant/static_visitor.hpp>

namespace vanetza
{
namespace geonet
{

VariantPdu::VariantPdu(const Pdu& pdu) :
    m_basic(pdu.basic()), m_common(pdu.common()), m_extended(pdu.extended_variant()),
    m_secured(pdu.secured() != nullptr, *pdu.secured())
{
}

VariantPdu& VariantPdu::operator=(const Pdu& pdu)
{
    m_basic = pdu.basic();
    m_common = pdu.common();
    m_extended = pdu.extended_variant();
    if (pdu.secured()) {
        m_secured = *pdu.secured();
    } else {
        m_secured = boost::none;
    }

    return *this;
}

BasicHeader& VariantPdu::basic()
{
    return m_basic;
}

const BasicHeader& VariantPdu::basic() const
{
    return m_basic;
}

CommonHeader& VariantPdu::common()
{
    return m_common;
}

const CommonHeader& VariantPdu::common() const
{
    return m_common;
}

HeaderVariant& VariantPdu::extended_variant()
{
    return m_extended;
}

HeaderConstRefVariant VariantPdu::extended_variant() const
{
    return m_extended;
}

VariantPdu::SecuredMessage* VariantPdu::secured()
{
    return m_secured.get_ptr();
}

const VariantPdu::SecuredMessage* VariantPdu::secured() const
{
    return m_secured.get_ptr();
}

void VariantPdu::secured(SecuredMessage* smsg)
{
    m_secured = boost::optional<SecuredMessage>(smsg, *smsg);
}

void VariantPdu::secured(SecuredMessage&& smsg)
{
    m_secured = std::move(smsg);
}

std::unique_ptr<Pdu> VariantPdu::clone() const
{
    return std::unique_ptr<Pdu> { new VariantPdu(*this) };
}

} // namespace geonet
} // namespace vanetza
