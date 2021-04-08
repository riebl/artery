package artery.envmod.service;

import artery.application.ItsG5Service;

simple CollectivePerceptionMockService like ItsG5Service
{
    parameters:
        @signal[CpmSent](type=CollectivePerceptionMockMessage);
        @signal[CpmReceived](type=CollectivePerceptionMockMessage);

        @statistic[transmission](source=CpmSent; record=count,vector(cpmSource)?);
        @statistic[reception](source=CpmReceived; record=count,vector(cpmSource)?);

        bool generateAfterCam = default(false);
        double cpmInterval @unit(s) = default(0.1s);
        double cpmOffset @unit(s) = default(0.05s); // offset to trigger event (CAM or middleware)
        double fovInterval @unit(s) = default(1s);

        int dccProfile = default(2);
        int lengthHeader @unit(byte) = default(37B);
        int lengthFovContainer @unit(byte) = default(9B);
        int lengthObjectContainer @unit(byte) = default(29B);
}
