package artery.envmod;

simple CarIdentityRegistrant
{
    parameters:
        @class(CarIdentityRegistrant);
        @signal[IdentityUpdated](type=artery::Identity);
        @signal[IdentityRemoved](type=artery::Identity);
        string stationIdDerivation = default("component");
}
