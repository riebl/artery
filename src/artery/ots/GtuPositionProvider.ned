package artery.ots;

import artery.networking.IPositionProvider;

module GtuPositionProvider like IPositionProvider
{
    parameters:
        @class(GtuPositionProvider);
        @signal[PositionFix](PositionFixObject);
        string runtimeModule;
        string mobilityModule;
}
