package artery.testbed;

import artery.testbed.OtaInterface;

simple OtaInterfaceUsrp like OtaInterface
{
    parameters:
        @class(OtaInterfaceUsrp);
        string hcmIp;
        int hcmPort = default(12345);
        int listeningPort = default(12346);
        int connectTimeout = default(5); // timeout for connection test to USRP
        bool fakeMode = default(false); // fake mode allows to operate without USRP

        bool openGpsdSocket = default(true);
        int gpsdPort = default(4006);
        double gpsdUpdateRate = default(0.2s) @unit(s);
        string datetime; // reference time for NMEA sentences
}
