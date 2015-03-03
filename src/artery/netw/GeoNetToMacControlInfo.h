//
// Copyright (C) 2014 Raphael Riebl <raphael.riebl@thi.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#ifndef GEONETTOMACCONTROLINFO_H_
#define GEONETTOMACCONTROLINFO_H_

#include <cobject.h>
#include "artery/mac/AccessCategories.h"
#include "veins/base/utils/SimpleAddress.h"

class GeoNetToMacControlInfo : public cObject
{
    public:
        LAddress::L2Type source_addr;
        LAddress::L2Type destination_addr;
        edca::AccessCategory access_category;
};

#endif /* GEONETTOMACCONTROLINFO_H_ */
