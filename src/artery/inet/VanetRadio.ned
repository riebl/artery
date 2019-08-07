package artery.inet;

import inet.physicallayer.ieee80211.packetlevel.Ieee80211ScalarRadio;

module VanetRadio extends Ieee80211ScalarRadio
{
    parameters:
        @class(VanetRadio);

        // signal on incoming radio frame
        @signal[RadioFrame](type=cPacket);
}
