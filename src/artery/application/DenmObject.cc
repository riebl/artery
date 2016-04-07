//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include <artery/application/DenmObject.h>
#include <cassert>

using Denm = vanetza::asn1::Denm;

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
        cause_code = static_cast<denm::CauseCode>(situation->eventType.causeCode);
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

bool operator&(const DenmObject& obj, denm::CauseCode cause)
{
    const auto obj_cause = obj.situation_cause_code();
    return (obj_cause && obj_cause.get() == cause);
}
