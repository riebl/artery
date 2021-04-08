package artery.application;

//
// InfrastructureMockService mimicks a service typically operated by a RSU.
// Its messages are disseminated as GeoBroadcast packets centred around the RSU's location.
// If the actual content of an RSU message is not relevant in your simulation, e.g. you are
// only interested in the impact on the channel congestion, you can employ this service to
// produce load similar to SPATEM and MAPEM messages.
//
simple InfrastructureMockService like ItsG5Service
{
    parameters:
        @signal[ImmSent](type=InfrastructureMockMessage);
        @statistic[transmission](source=ImmSent; record=count,vector(immSource)?,vector(immSequenceNumber)?);

        // generation interval
        double generationInterval @unit(s) = default(0.1s);
        double generationOffset @unit(s) = default(uniform(0.0s, 1.0s));

        int messageLengthMin = default(40);
        int messageLengthMax = default(1500);
        int messageLength = default(int(lognormal(5.4, 0.3)));

        int packetPriority = default(3);
        double disseminationRadius @unit(m) = default(400m);
}
