package artery.inet;

import inet.linklayer.ieee80211.Ieee80211Interface;

module VanetNic extends Ieee80211Interface
{
    parameters:
        mgmt.typename = "Ieee80211MgmtAdhoc";
        agent.typename = "";
        opMode = "p";

        mac.qosStation = true;
        mac.rx.typename = "VanetRx";

        radio.bandName = "5.9 GHz";
        radio.bandwidth = 10 MHz;
        radio.channelNumber = default(4);
        radio.carrierFrequency = default(5.9 GHz);

        mac.hcf.rateSelection.dataFrameBitrate = default(6 Mbps);
        mac.hcf.rateSelection.mgmtFrameBitrate = default(6 Mbps);
        mac.hcf.rateSelection.multicastFrameBitrate = default(6 Mbps);

        // EDCA settings for activated OCB mode (see table 8-106 in IEEE 802.11-2012)
        mac.hcf.edcaTxopProcedures[*].txopLimit = 0;

        // AC_BK
        mac.hcf.edca.edcaf[0].aifsn = 9;
        mac.hcf.edca.edcaf[0].cwMin = 15;
        mac.hcf.edca.edcaf[0].cwMax = 1023;

        // AC_BE
        mac.hcf.edca.edcaf[1].aifsn = 6;
        mac.hcf.edca.edcaf[1].cwMin = 15;
        mac.hcf.edca.edcaf[1].cwMax = 1023;

        // AC_VI
        mac.hcf.edca.edcaf[2].aifsn = 3;
        mac.hcf.edca.edcaf[2].cwMin = 7;
        mac.hcf.edca.edcaf[2].cwMax = 15;

        // AC_VO
        mac.hcf.edca.edcaf[3].aifsn = 2;
        mac.hcf.edca.edcaf[3].cwMin = 3;
        mac.hcf.edca.edcaf[3].cwMax = 7;
}
