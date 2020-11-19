package artery.application;

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
