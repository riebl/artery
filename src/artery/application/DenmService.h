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

#ifndef __ARTERY_DENMSERVICE_H_
#define __ARTERY_DENMSERVICE_H_

#include "artery/application/ItsG5BaseService.h"
#include <vanetza/btp/data_indication.hpp>

class DenmService : public ItsG5BaseService
{
    public:
        void indicate(const vanetza::btp::DataIndication&, std::unique_ptr<vanetza::btp::UpPacket>) override;
};

#endif
