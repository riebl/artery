package artery.application;

simple LocationTableLogger like ItsG5Service
{
    parameters:
        @signal[CommunicationRange](type=double);
        @statistic[CommunicationRange](record=max,vector?);

        @signal[NeighbourCount](type=unsigned long);
        @statistic[NeighbourCount](record=vector?);

        @signal[NeighbourDensity](type=unsigned long);
        @statistic[NeighbourDensity](record=vector?);

        string routerModule = default(".vanetza[0].router");
}
