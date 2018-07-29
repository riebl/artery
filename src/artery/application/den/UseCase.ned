package artery.application.den;

moduleinterface UseCase
{
}

moduleinterface SuspendableUseCase extends UseCase
{
    double detectionBlockingInterval @unit(s);
}
