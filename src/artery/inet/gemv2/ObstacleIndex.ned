package artery.inet.gemv2;

simple ObstacleIndex
{
    parameters:
        @class(gemv2::ObstacleIndex);
        string traciModule;
        string visualizerModule;
        string filterTypes = default("building");
        string obstacleColor = default("Black");
        bool requireFilled = default(false);
}
