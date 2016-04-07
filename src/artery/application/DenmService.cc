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

#include "artery/application/Asn1PacketVisitor.h"
#include "artery/application/DenmObject.h"
#include "artery/application/DenmService.h"
#include <vanetza/asn1/denm.hpp>

Define_Module(DenmService);

static const simsignal_t scSignalDenmReceived = cComponent::registerSignal("DenmService.received");

void DenmService::indicate(const vanetza::btp::DataIndication& indication, std::unique_ptr<vanetza::UpPacket> packet)
{
    Asn1PacketVisitor<vanetza::asn1::Denm> visitor;
    vanetza::asn1::Denm* denm = boost::apply_visitor(visitor, *packet);
    if (denm) {
        DenmObject obj = std::move(*denm);
        emit(scSignalDenmReceived, &obj);
    }
}
