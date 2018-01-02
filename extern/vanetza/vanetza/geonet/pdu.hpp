#ifndef PDU_HPP_PQEC9PDO
#define PDU_HPP_PQEC9PDO

#include <vanetza/geonet/header_variant.hpp>
#include <vanetza/geonet/serialization.hpp>
#include <cstddef>
#include <memory>

namespace vanetza
{
// forward declarations
namespace security { struct SecuredMessageV2; }

namespace geonet
{

struct BasicHeader;
struct CommonHeader;

class Pdu;

class ConstAccessiblePdu
{
public:
    using SecuredMessage = security::SecuredMessageV2;

    virtual const BasicHeader& basic() const = 0;
    virtual const CommonHeader& common() const = 0;
    virtual const SecuredMessage* secured() const = 0;
    virtual HeaderConstRefVariant extended_variant() const = 0;
    virtual std::unique_ptr<Pdu> clone() const = 0;
    virtual ~ConstAccessiblePdu() = default;
};

class Pdu : public ConstAccessiblePdu
{
public:
    using ConstAccessiblePdu::basic;
    using ConstAccessiblePdu::common;
    using ConstAccessiblePdu::secured;

    virtual BasicHeader& basic() = 0;
    virtual CommonHeader& common() = 0;
    virtual SecuredMessage* secured() = 0;
    virtual void secured(SecuredMessage*) = 0;
    virtual void secured(SecuredMessage&&) = 0;
};

void serialize(const ConstAccessiblePdu&, OutputArchive&);
inline void serialize(const Pdu& pdu, OutputArchive& ar)
{
    serialize(static_cast<const ConstAccessiblePdu&>(pdu), ar);
}

std::size_t get_length(const ConstAccessiblePdu&);

} // namespace geonet
} // namespace vanetza

#endif /* PDU_HPP_PQEC9PDO */

