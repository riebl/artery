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

#ifndef __ARTERY_DENMOBJECT_H_
#define __ARTERY_DENMOBJECT_H_

#include <cobject.h>
#include <vanetza/asn1/denm.hpp>

class DenmObject : public cObject, public vanetza::asn1::Denm
{
    public:
        DenmObject(vanetza::asn1::Denm&&);
};

#endif /* __ARTERY_DENMOBJECT_H_ */
