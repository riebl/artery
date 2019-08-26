package artery.inet;

import inet.linklayer.ieee80211.mac.Rx;

simple PowerLevelRx extends Rx
{
    parameters:
        @class(PowerLevelRx);

        // ChannelLoad signal reporting Channel Busy Ratio (CBR) between 0.0 and 1.0
        @signal[ChannelLoad](type=double);
        @statistic[ChannelLoad](record=timeavg,vector?);

        string radioModule = default("^.^.radio");

        // optionally synchronise channel reports across nodes at integer report intervals
        bool asyncChannelReport = default(true);

        // include duration of own transmissions as busy in channel load reports
        bool cbrWithTx = default(false);

        // ETSI ITS threshold for CBR measuremnts
        double cbrThreshold @unit(dBm) = default(-85 dBm);

        // IEEE 802.11 CCA thresholds for OFDM signals and noise power
        double ccaSignalThreshold @unit(dBm) = default(-85 dBm);
        double ccaNoiseThreshold @unit(dBm) = default(-65 dBm);
}
