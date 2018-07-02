package artery.application.den;

simple TrafficJamEndOfQueue extends UseCase
{
    @class(artery::den::TrafficJamEndOfQueue);
    double sampleDuration @unit(s) = default(10s);
    double sampleInterval @unit(s) = default(100ms);
    bool nonUrbanEnvironment = default(true);
    detectionBlockingTime = default(60s);
}

simple TrafficJamAhead extends UseCase
{
    @class(artery::den::TrafficJamAhead);
    double sampleDuration @unit(s) = default(120s);
    double sampleInterval @unit(s) = default(1s);
    bool nonUrbanEnvironment = default(true);
    detectionBlockingTime = default(60s);
}