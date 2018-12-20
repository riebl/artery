package artery.networking;

simple StationaryPositionProvider like IPositionProvider
{
    parameters:
        @class(StationaryPositionProvider);
        @signal[PositionFix](PositionFixObject);
        string mobilityModule;
        string runtimeModule;
        string traciCoreModule;
}
