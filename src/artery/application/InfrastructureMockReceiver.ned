package artery.application;

//
// InfrastructureMockReceiver can collect reception statistics for messages emitted by InfrastructureMockService
//
simple InfrastructureMockReceiver like ItsG5Service
{
    parameters:
        @signal[ImmReceived](type=InfrastructureMockMessage);
        @statistic[reception](source=ImmReceived;record=count,vector(immSource)?,vector(immSequenceNumber)?);
}
