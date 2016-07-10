package artery.inet;

import inet.linklayer.ieee80211.mac.Rx;

simple VanetRx extends Rx
{
    parameters:
        @class(VanetRx);
        @signal[ChannelLoad](type=double);
}
