#ifndef ARTERY_CPOBJECT_H_
#define ARTERY_CPOBJECT_H_

#include <omnetpp/cobject.h>
#include <vanetza/asn1/cpm.hpp>

#include <memory>

namespace artery
{

using Cpm = vanetza::asn1::r2::Cpm;

class CpObject : public omnetpp::cObject
{
public:
    CpObject(const CpObject&) = default;
    CpObject& operator=(const CpObject&) = default;

    CpObject(Cpm&&);
    CpObject& operator=(Cpm&&);

    CpObject(const Cpm&);
    CpObject& operator=(const Cpm&);

    CpObject(const std::shared_ptr<const Cpm>&);
    CpObject& operator=(const std::shared_ptr<const Cpm>&);

    const Cpm& asn1() const;

    std::shared_ptr<const Cpm> shared_ptr() const;

    omnetpp::cObject* dup() const override;

private:
    std::shared_ptr<const Cpm> m_cpm_wrapper;
};

}  // namespace artery

#endif /* ARTERY_CPOBJECT_H_ */
