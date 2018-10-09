package artery.networking;

simple VehiclePositionProvider like IPositionProvider
{
    parameters:
        @class(artery::VehiclePositionProvider);
        @signal[PositionFix](PositionFixObject);
        string mobilityModule;
        string runtimeModule;
}
