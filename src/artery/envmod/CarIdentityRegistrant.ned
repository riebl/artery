package artery.envmod;

simple CarIdentityRegistrant
{
    parameters:
        @class(CarIdentityRegistrant);
        @signal[Identity.update](type=artery::Identity);
        @signal[Identity.remove](type=artery::Identity);
}
