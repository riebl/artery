#include <artery/application/CaObject.h>
#include <omnetpp.h>
#include <cassert>

using namespace vanetza::asn1;

Register_Abstract_Class(CaObject)

CaObject::CaObject(Cam&& cam) :
    m_cam_wrapper(std::make_shared<Cam>(std::move(cam)))
{
}

CaObject& CaObject::operator=(Cam&& cam)
{
    m_cam_wrapper = std::make_shared<Cam>(std::move(cam));
    return *this;
}

CaObject::CaObject(const Cam& cam) :
    m_cam_wrapper(std::make_shared<Cam>(cam))
{
}

CaObject& CaObject::operator=(const Cam& cam)
{
    m_cam_wrapper = std::make_shared<Cam>(cam);
    return *this;
}

CaObject::CaObject(const std::shared_ptr<Cam>& ptr) :
    m_cam_wrapper(ptr)
{
    assert(m_cam_wrapper);
}

CaObject& CaObject::operator=(const std::shared_ptr<Cam>& ptr)
{
    m_cam_wrapper = ptr;
    assert(m_cam_wrapper);
    return *this;
}

std::shared_ptr<const Cam> CaObject::shared_ptr() const
{
    assert(m_cam_wrapper);
    return m_cam_wrapper;
}

vanetza::asn1::Cam& CaObject::asn1()
{
    return *m_cam_wrapper;
}

const vanetza::asn1::Cam& CaObject::asn1() const
{
    return *m_cam_wrapper;
}
