package artery.application.den;

simple TrafficJamEndOfQueue like SuspendableUseCase
{
    @class(den::TrafficJamEndOfQueue);
    double sampleDuration @unit(s) = default(10s);
    double sampleInterval @unit(s) = default(100ms);
    bool nonUrbanEnvironment = default(true);
    double detectionBlockingInterval @unit(s) = default(60s);
}

simple TrafficJamAhead like SuspendableUseCase
{
    @class(den::TrafficJamAhead);
    double sampleDuration @unit(s) = default(120s);
    double sampleInterval @unit(s) = default(1s);
    bool nonUrbanEnvironment = default(true);
    double detectionBlockingInterval @unit(s) = default(60s);
}
