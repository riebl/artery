package artery.application.den;

simple TractionLoss like SuspendableUseCase
{
    @class(den::TractionLoss);
    double detectionBlockingInterval @unit(s) = default(20s);
}
