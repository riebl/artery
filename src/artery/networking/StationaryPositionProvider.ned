package artery.networking;

simple StationaryPositionProvider like IPositionProvider
{
    parameters:
        @class(StationaryPositionProvider);
        string mobilityModule;
        string runtimeModule;
        string traciCoreModule;
}
