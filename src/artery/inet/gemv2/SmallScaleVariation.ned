package artery.inet.gemv2;

simple SmallScaleVariation
{
    parameters:
        @class(artery::gemv2::SmallScaleVariation);
        string obstacleIndexModule;
        string vehicleIndexModule;

        double minStdDevLOS @unit(dB) = default(3.3 dB);
        double minStdDevNLOSv @unit(dB) = default(3.8 dB);
        double minStdDevNLOSb @unit(dB) = default(4.1 dB);

        double maxStdDevLOS @unit(dB) = default(5.2 dB);
        double maxStdDevNLOSv @unit(dB) = default(5.3 dB);
        double maxStdDevNLOSb @unit(dB) = default(6.8 dB);

        double maxVehicleDensity;
        double maxObstacleDensity;
}
