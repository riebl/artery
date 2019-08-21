package artery.inet;

import inet.physicallayer.ieee80211.packetlevel.Ieee80211ScalarReceiver;

module VanetReceiver extends Ieee80211ScalarReceiver
{
    parameters:
        @class(VanetReceiver);
        double captureThreshold @unit(dB) = default(6 dB);
}
