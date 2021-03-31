package artery.networking;

simple PersonPositionProvider like IPositionProvider
{
    parameters:
        @class(PersonPositionProvider);
        @signal[PositionFix](PositionFixObject);
        string mobilityModule;
        string runtimeModule;
}
