#include <artery/application/CaObject.h>
#include <omnetpp.h>
#include <cassert>

namespace artery
{

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

CaObject::CaObject(const std::shared_ptr<const Cam>& ptr) :
    m_cam_wrapper(ptr)
{
    assert(m_cam_wrapper);
}

CaObject& CaObject::operator=(const std::shared_ptr<const Cam>& ptr)
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

const vanetza::asn1::Cam& CaObject::asn1() const
{
    return *m_cam_wrapper;
}

omnetpp::cObject* CaObject::dup() const
{
    return new CaObject { *this };
}

using namespace omnetpp;

class CamStationIdResultFilter : public cObjectResultFilter
{
protected:
    void receiveSignal(cResultFilter* prev, simtime_t_cref t, cObject* object, cObject* details) override
    {
        if (auto cam = dynamic_cast<CaObject*>(object)) {
            const auto id = cam->asn1()->header.stationID;
            fire(this, t, id, details);
        }
    }
};

Register_ResultFilter("camStationId", CamStationIdResultFilter)


class CamGenerationDeltaTimeResultFilter : public cObjectResultFilter
{
protected:
    void receiveSignal(cResultFilter* prev, simtime_t_cref t, cObject* object, cObject* details) override
    {
        if (auto cam = dynamic_cast<CaObject*>(object)) {
            const auto genDeltaTime = cam->asn1()->cam.generationDeltaTime;
            fire(this, t, genDeltaTime, details);
        }
    }
};

Register_ResultFilter("camGenerationDeltaTime", CamGenerationDeltaTimeResultFilter)

} // namespace artery
