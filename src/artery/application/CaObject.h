#ifndef ARTERY_CAOBJECT_H_
#define ARTERY_CAOBJECT_H_

#include <omnetpp/cobject.h>
#include <vanetza/asn1/cam.hpp>
#include <memory>

class CaObject : public omnetpp::cObject
{
public:
    CaObject(const CaObject&) = default;
    CaObject& operator=(const CaObject&) = default;

    CaObject(vanetza::asn1::Cam&&);
    CaObject& operator=(vanetza::asn1::Cam&&);

    CaObject(const vanetza::asn1::Cam&);
    CaObject& operator=(const vanetza::asn1::Cam&);

    CaObject(std::shared_ptr<const vanetza::asn1::Cam>);
    CaObject& operator=(std::shared_ptr<const vanetza::asn1::Cam>);

    CaObject(std::shared_ptr<vanetza::asn1::Cam>);
    CaObject& operator=(std::shared_ptr<vanetza::asn1::Cam>);

    std::shared_ptr<const vanetza::asn1::Cam> asn1() const;

private:
    std::shared_ptr<const vanetza::asn1::Cam> m_cam_wrapper;
};

#endif /* ARTERY_CAOBJECT_H_ */
