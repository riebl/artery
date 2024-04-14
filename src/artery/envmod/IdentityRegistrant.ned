package artery.envmod;

simple IdentityRegistrant
{
    parameters:
        @class(IdentityRegistrant);
        @signal[IdentityUpdated](type=artery::Identity);
        @signal[IdentityRemoved](type=artery::Identity);

        string mobilityModule = default("^.mobility");
        string stationIdDerivation = default("component");
}
