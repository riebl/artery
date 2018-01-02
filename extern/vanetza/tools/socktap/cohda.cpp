#include "cohda.hpp"
#include <vanetza/dcc/data_request.hpp>
#include <cassert>
#include <mk2mac-api-types.h>

namespace vanetza
{

using dcc::Profile;

ByteBuffer create_cohda_tx_header(const dcc::DataRequest& request)
{
    ByteBuffer header(sizeof(tMK2TxDescriptor), 0x00);
    assert(header.size() == sizeof(tMK2TxDescriptor));
    tMK2TxDescriptor* tx = reinterpret_cast<tMK2TxDescriptor*>(header.data());
    tx->ChannelNumber = 180;
    switch (request.dcc_profile) {
        case Profile::DP0:
            tx->Priority = 7; // AC_VO
            break;
        case Profile::DP1:
            tx->Priority = 5; // AC_VI
            break;
        case Profile::DP2:
            tx->Priority = 3; // AC_BE
            break;
        case Profile::DP3:
            tx->Priority = 1; // AC_BK
            break;
        default:
            tx->Priority = MK2_PRIO_NON_QOS;
            break;
    }
    tx->Service = MK2_QOS_ACK;
    tx->MCS = MK2MCS_DEFAULT;
    tx->TxPower.PowerSetting = MK2TPC_DEFAULT;
    tx->TxAntenna = MK2_TXANT_DEFAULT;
    return header;
}

} // namespace vanetza
