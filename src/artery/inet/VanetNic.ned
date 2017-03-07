package artery.inet;

import inet.linklayer.ieee80211.Ieee80211Nic;

module VanetNic extends Ieee80211Nic
{
    parameters:
        macType = "Ieee80211Mac";
        mgmtType = "Ieee80211MgmtAdhoc";
        opMode = "p";
        bitrate = 6 Mbps;
        mac.rx.typename = "VanetRx";
        mac.rateSelection.modeSet = "p";
        mac.rateSelection.dataBitrate = 6 Mbps;
        mac.rateSelection.multicastBitrate = 6 Mbps;
        radio.bandName = "5 GHz";
        radio.bandwidth = 10 MHz;
        radio.channelNumber = 180;
        radio.carrierFrequency = 5.9 GHz;
}
