#include <artery/application/CaObject.h>
#include <omnetpp.h>
#include <cassert>

using namespace vanetza::asn1;

Register_Abstract_Class(CaObject)

CaObject::CaObject(Cam&& cam) :
    m_cam_wrapper(std::make_shared<Cam>(std::move(cam)))
{
}

CaObject::CaObject(const Cam& cam) :
    m_cam_wrapper(std::make_shared<Cam>(cam))
{
}

CaObject::CaObject(const std::shared_ptr<Cam>& cam) :
    m_cam_wrapper(cam)
{
    assert(m_cam_wrapper);
}

std::shared_ptr<Cam> CaObject::asn1()
{
    return m_cam_wrapper;
}
