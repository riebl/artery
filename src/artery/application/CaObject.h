#ifndef ARTERY_CAOBJECT_H_
#define ARTERY_CAOBJECT_H_

#include <omnetpp/cobject.h>
#include <vanetza/asn1/cam.hpp>
#include <memory>

namespace artery
{

class CaObject : public omnetpp::cObject
{
public:
    CaObject(const CaObject&) = default;
    CaObject& operator=(const CaObject&) = default;

    CaObject(vanetza::asn1::Cam&&);
    CaObject& operator=(vanetza::asn1::Cam&&);

    CaObject(const vanetza::asn1::Cam&);
    CaObject& operator=(const vanetza::asn1::Cam&);

    CaObject(const std::shared_ptr<const vanetza::asn1::Cam>&);
    CaObject& operator=(const std::shared_ptr<const vanetza::asn1::Cam>&);

    const vanetza::asn1::Cam& asn1() const;

    std::shared_ptr<const vanetza::asn1::Cam> shared_ptr() const;

    omnetpp::cObject* dup() const override;

private:
    std::shared_ptr<const vanetza::asn1::Cam> m_cam_wrapper;
};

} // namespace artery

#endif /* ARTERY_CAOBJECT_H_ */
