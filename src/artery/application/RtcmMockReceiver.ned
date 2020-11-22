package artery.application;

//
// RtcmMockReceiver can collect reception statistics for messages emitted by RtcmMockService
//
simple RtcmMockReceiver like ItsG5Service
{
    parameters:
        @signal[RtcmReceived](type=RtcmMockMessage);
        @statistic[reception](source=RtcmReceived;record=count,vector(rtcmSource)?,vector(rtcmGenerated)?);
}
