package artery.networking;

simple StationaryPositionProvider like IPositionProvider
{
    parameters:
        @class(artery::StationaryPositionProvider);
        string mobilityModule;
        string runtimeModule;
        string traciCoreModule;
}
