package artery.application;

simple GbcMockService like ItsG5Service
{
    parameters:
        @signal[GbcMockReceived](type=GbcMockMessage);
        @signal[GbcMockSent](type=GbcMockMessage);

        @statistic[reception](source=GbcMockReceived;record=vector(gbcLatency)?,vector(gbcRange)?);
        @statistic[transmission](source=GbcMockSent;record=vector(constant1)?);
        @statistic[uniqueReception](source=gbcUniqueReception(GbcMockReceived);record=vector(gbcLatency)?,vector(gbcRange)?);

        volatile double generationInterval @unit(s) = default(1s);
        double generationOffset @unit(s) = default(uniform(0.0s, 1.0s));
        string generationSignal = default("");
        volatile int payloadLength @unit(byte) = default(500B);
        double gnLifetime @unit(s) = default(5s);
        double gnRadius @unit(m) = default(1000m);
        bool gnStoreCarryForwarding = default(false);
        double gnRepetitionInterval @unit(s) = default(0s);
        double gnRepetitionMaximum @unit(s) = default(0s);
        int dccProfile = default(3);
        int packetLimit = default(-1);
}
