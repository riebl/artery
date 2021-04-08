package artery.application;

//
// RtcmMockService is intended for evaluating the dissemination of Cooperative Positioning messages.
//
// An earlier variant of this service has been employed in
//    "VANET Coverage Analysis for GPS Augmentation Data in Rural Area"
//    https://doi.org/10.1016/j.ifacol.2016.11.112
//
simple RtcmMockService like ItsG5Service
{
    parameters:
        @signal[RtcmSent](type=RtcmMockMessage);
        @statistic[transmission](source=RtcmSent;record=count,vector(rtcmSource)?,vector(rtcmGenerated)?);

        // generation interval
        double generationInterval @unit(s) = default(1s);
        double generationOffset @unit(s) = default(uniform(0.0s, 1.0s));

        int rtcmLength @unit(byte) = default(372B);
        double gnLifetime @unit(s) = default(5s);
        double gnRadius @unit(m) = default(1000m);
        bool gnStoreCarryForwarding = default(false);
        int dccProfile = default(3);
}
