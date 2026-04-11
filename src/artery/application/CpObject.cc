#include <artery/application/CpObject.h>
#include <omnetpp.h>
#include <cassert>

namespace artery
{

Register_Abstract_Class(CpObject)

CpObject::CpObject(Cpm&& cpm) :
    m_cpm_wrapper(std::make_shared<Cpm>(std::move(cpm)))
{
}

CpObject& CpObject::operator=(Cpm&& cpm)
{
    m_cpm_wrapper = std::make_shared<Cpm>(std::move(cpm));
    return *this;
}

CpObject::CpObject(const Cpm& cpm) :
    m_cpm_wrapper(std::make_shared<Cpm>(cpm))
{
}

CpObject& CpObject::operator=(const Cpm& cpm)
{
    m_cpm_wrapper = std::make_shared<Cpm>(cpm);
    return *this;
}

CpObject::CpObject(const std::shared_ptr<const Cpm>& ptr) :
    m_cpm_wrapper(ptr)
{
    assert(m_cpm_wrapper);
}

CpObject& CpObject::operator=(const std::shared_ptr<const Cpm>& ptr)
{
    m_cpm_wrapper = ptr;
    assert(m_cpm_wrapper);
    return *this;
}

std::shared_ptr<const Cpm> CpObject::shared_ptr() const
{
    assert(m_cpm_wrapper);
    return m_cpm_wrapper;
}

const Cpm& CpObject::asn1() const
{
    return *m_cpm_wrapper;
}

omnetpp::cObject* CpObject::dup() const
{
    return new CpObject { *this };
}

using namespace omnetpp;

class CpmStationIdResultFilter : public cObjectResultFilter
{
protected:
    void receiveSignal(cResultFilter* prev, simtime_t_cref t, cObject* object, cObject* details) override
    {
        if (auto cpm = dynamic_cast<CpObject*>(object)) {
            const auto id = cpm->asn1()->header.stationId;
            fire(this, t, id, details);
        }
    }
};

Register_ResultFilter("cpmStationId", CpmStationIdResultFilter)


class CpmReferenceTimeResultFilter : public cObjectResultFilter
{
protected:
    void receiveSignal(cResultFilter* prev, simtime_t_cref t, cObject* object, cObject* details) override
    {
        if (auto cpm = dynamic_cast<CpObject*>(object)) {
            // 42 bit sequence number (0..4398046511103). Minimum 6 bytes required.
            static_assert(sizeof(unsigned long) >= 6, "unsigned long cannot represent ReferenceTime");
            const auto& referenceTime = cpm->asn1()->payload.managementContainer.referenceTime;
            unsigned long genTime;
            if(asn_INTEGER2ulong(&referenceTime, &genTime) == 0) {
                // genTime = genTime | 0x3FFFFFFFFFF;
                fire(this, t, genTime, details);
            }
        }
    }
};

Register_ResultFilter("cpmReferenceTime", CpmReferenceTimeResultFilter)

} // namespace artery
