package artery.inet;

import inet.linklayer.ieee80211.mac.Rx;

simple ChannelLoadRx extends Rx
{
    parameters:
        @class(ChannelLoadRx);
        @signal[ChannelLoad](type=double);
        @statistic[ChannelLoad](record=timeavg,vector?);

        bool cbrWithTx = default(false);
        bool asyncChannelReport = default(true);
}
