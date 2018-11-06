/*
* Artery V2X Simulation Framework
* Copyright 2015-2019 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/application/ItsG5PromiscuousService.h"

namespace artery
{

void ItsG5PromiscuousService::tap(const vanetza::btp::DataIndication& indication, const vanetza::UpPacket& packet, const NetworkInterface& interface)
{
    this->tapPacket(indication, packet);
}

} // namespace artery
