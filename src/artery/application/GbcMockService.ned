package artery.application;

//
// GbcMockService produces generic GeoBroadcast packets.
//
// Many GeoNetworking options can be adjusted to evaluate their impact on the network.
// The packets are generated periodically according to the specified interval and offset.
// Optionally, packet generation can be triggered by a storyboard signal.
// If the packet limit is set to zero, no packets are generated periodically but only by explict signals.
// By default, the destination area is centred at the station's current position.
// This behaviour can be overriden by defining the destination area's centre point explicitly.
//
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
        int packetLimit = default(-1); // -1 means unlimited

        bool disseminateAroundStation = default(true);
        double gnDestinationLongitude @unit(deg) = default(0.0 deg);
        double gnDestinationLatitude @unit(deg) = default(0.0 deg);
}
