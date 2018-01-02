#ifndef VARIANT_PDU_HPP_PHYRXMBW
#define VARIANT_PDU_HPP_PHYRXMBW

#include <vanetza/geonet/basic_header.hpp>
#include <vanetza/geonet/common_header.hpp>
#include <vanetza/geonet/header_variant.hpp>
#include <vanetza/geonet/pdu.hpp>
#include <vanetza/security/secured_message.hpp>
#include <boost/optional.hpp>

namespace vanetza
{
namespace geonet
{

class VariantPdu : public Pdu
{
public:
    using SecuredMessage = security::SecuredMessage;

    VariantPdu() = default;
    VariantPdu(const Pdu&);
    VariantPdu& operator=(const Pdu&);

    BasicHeader& basic() override;
    const BasicHeader& basic() const override;
    CommonHeader& common() override;
    const CommonHeader& common() const override;
    HeaderVariant& extended_variant();
    HeaderConstRefVariant extended_variant() const override;
    SecuredMessage* secured() override;
    const SecuredMessage* secured() const override;
    void secured(SecuredMessage*) override;
    void secured(SecuredMessage&&) override;
    std::unique_ptr<Pdu> clone() const override;

private:
    BasicHeader m_basic;
    CommonHeader m_common;
    HeaderVariant m_extended;
    boost::optional<SecuredMessage> m_secured;
};

} // namespace geonet
} // namespace vanetza

#endif /* VARIANT_PDU_HPP_PHYRXMBW */

