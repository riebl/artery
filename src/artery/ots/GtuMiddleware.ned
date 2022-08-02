package artery.ots;

import artery.application.Middleware;

simple GtuMiddleware extends Middleware
{
    parameters:
        @class(GtuMiddleware);
        @signal[IdentityChanged](type=long);

        string localEnvironmentModule = default("");
        string globalEnvironmentModule = default("");
        string mobilityModule;
        string stationIdDerivation = default("component");
}

