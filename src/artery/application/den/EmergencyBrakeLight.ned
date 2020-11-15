package artery.application.den;

simple EmergencyBrakeLight like SuspendableUseCase
{
    @class(den::EmergencyBrakeLight);
    double sampleDuration @unit(s) = default(500ms);
    double sampleInterval @unit(s) = default(100ms);
    double speedThreshold @unit(mps) = default(20kmph);
    double decelerationThreshold = default(-7.0);
    double detectionBlockingInterval @unit(s) = default(100ms);
}
