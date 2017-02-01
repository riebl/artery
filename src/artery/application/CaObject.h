#ifndef ARTERY_CAOBJECT_H_
#define ARTERY_CAOBJECT_H_

#include <omnetpp/cobject.h>
#include <vanetza/asn1/cam.hpp>
#include <memory>

class CaObject : public omnetpp::cObject
{
public:
    CaObject(vanetza::asn1::Cam&&);
    CaObject(const vanetza::asn1::Cam&);
    CaObject(const std::shared_ptr<vanetza::asn1::Cam>&);
    std::shared_ptr<vanetza::asn1::Cam> asn1();

private:
    std::shared_ptr<vanetza::asn1::Cam> m_cam_wrapper;
};

#endif /* ARTERY_CAOBJECT_H_ */
