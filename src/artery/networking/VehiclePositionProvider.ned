package artery.networking;

simple VehiclePositionProvider like IPositionProvider
{
    parameters:
        @class(VehiclePositionProvider);
        @signal[PositionFix](PositionFixObject);
        string mobilityModule;
        string runtimeModule;
}
