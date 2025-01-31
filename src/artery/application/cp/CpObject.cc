/*
 * Artery V2X Simulation Framework
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/cp/CpObject.h"

#include <omnetpp.h>

#include <cassert>


namespace artery
{

Register_Abstract_Class(CpObject);

CpObject::CpObject(vanetza::asn1::Cpm&& cpm) : m_wrapper(std::make_shared<vanetza::asn1::Cpm>(std::move(cpm)))
{
}

CpObject& CpObject::operator=(vanetza::asn1::Cpm&& cpm)
{
    m_wrapper = std::make_shared<vanetza::asn1::Cpm>(std::move(cpm));
    return *this;
}


CpObject::CpObject(const vanetza::asn1::Cpm& cpm) : m_wrapper(std::make_shared<vanetza::asn1::Cpm>(cpm))
{
}

CpObject& CpObject::operator=(const vanetza::asn1::Cpm& cpm)
{
    m_wrapper = std::make_shared<vanetza::asn1::Cpm>(cpm);
    return *this;
}


CpObject::CpObject(std::shared_ptr<const vanetza::asn1::Cpm> ptr) : m_wrapper(ptr)
{
    assert(m_wrapper);
}

CpObject& CpObject::operator=(std::shared_ptr<const vanetza::asn1::Cpm> ptr)
{
    m_wrapper = ptr;
    assert(m_wrapper);
    return *this;
}


const vanetza::asn1::Cpm& CpObject::asn1() const
{
    return *m_wrapper;
}


std::shared_ptr<const vanetza::asn1::Cpm> CpObject::shared_ptr()
{
    return m_wrapper;
}


omnetpp::cObject* CpObject::dup() const
{
    return new CpObject{*this};
}

}  // namespace artery
