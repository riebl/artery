/*
 * Artery V2X Simulation Framework
 * Copyright 2016-2017 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/DenmObject.h"
#include <omnetpp.h>
#include <cassert>

using Denm = vanetza::asn1::Denm;

Register_Abstract_Class(artery::DenmObject)

namespace artery
{
namespace denm
{

CauseCode convert(const CauseCodeType_t& type)
{
    return static_cast<CauseCode>(type);
}

} // namespace denm


DenmObject::DenmObject(Denm&& denm) :
    m_denm_wrapper(std::make_shared<Denm>(std::move(denm)))
{
}

DenmObject::DenmObject(const Denm& denm) :
    m_denm_wrapper(std::make_shared<Denm>(denm))
{
}

DenmObject::DenmObject(const std::shared_ptr<Denm>& denm) :
    m_denm_wrapper(denm)
{
    assert(m_denm_wrapper);
}

boost::optional<denm::CauseCode> DenmObject::situation_cause_code() const
{
    boost::optional<denm::CauseCode> cause_code;
    const SituationContainer* situation = asn1()->denm.situation;
    if (situation) {
        cause_code = denm::convert(situation->eventType.causeCode);
    }
    return cause_code;
}

Denm& DenmObject::asn1()
{
    return *m_denm_wrapper;
}

const Denm& DenmObject::asn1() const
{
    return *m_denm_wrapper;
}

std::shared_ptr<const Denm> DenmObject::shared_ptr() const
{
    return m_denm_wrapper;
}

bool operator&(const DenmObject& obj, denm::CauseCode cause)
{
    const auto obj_cause = obj.situation_cause_code();
    return (obj_cause && obj_cause.get() == cause);
}

} // namespace artery
