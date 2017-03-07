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

        // EDCA settings for activated OCB mode (see table 8-106 in IEEE 802.11-2012)
        // AC_BK
        mac.upperMac.txopLimit0 = 0;
        mac.upperMac.aifsn0 = 9;
        mac.upperMac.cwMin0 = 15;
        mac.upperMac.cwMax0 = 1023;

        // AC_BE
        mac.upperMac.txopLimit1 = 0;
        mac.upperMac.aifsn1 = 6;
        mac.upperMac.cwMin1 = 15;
        mac.upperMac.cwMax1 = 1023;

        // AC_VI
        mac.upperMac.txopLimit2 = 0;
        mac.upperMac.aifsn2 = 3;
        mac.upperMac.cwMin2 = 7;
        mac.upperMac.cwMax2 = 15;

        // AC_VO
        mac.upperMac.txopLimit3 = 0;
        mac.upperMac.aifsn3 = 2;
        mac.upperMac.cwMin3 = 3;
        mac.upperMac.cwMax3 = 7;
}
