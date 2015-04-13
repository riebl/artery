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

DenmObject::DenmObject(vanetza::asn1::Denm&& denm) :
    vanetza::asn1::Denm(std::move(denm))
{
}

bool operator&(const DenmObject& obj, denm::CauseCode cause)
{
    bool is_cause_code = false;
    if (obj->denm.situation) {
        is_cause_code = (obj->denm.situation->eventType.causeCode == static_cast<CauseCodeType_t>(cause));
    }
    return is_cause_code;
}
