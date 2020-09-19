package artery.inet.gemv2;

simple SmallScaleVariation
{
    parameters:
        @class(gemv2::SmallScaleVariation);
        string obstacleIndexModule;
        string vehicleIndexModule;

        // default values are taken from http://mateboban.net/mboban_GEMV2_IEEE_TVT.pdf (Table V)
        double minStdDevLOS @unit(dB) = default(3.3 dB);
        double maxStdDevLOS @unit(dB) = default(5.2 dB);

        double minStdDevNLOSv @unit(dB) = default(3.8 dB);
        double maxStdDevNLOSv @unit(dB) = default(5.3 dB);

        // use 0 dB for minStdDev when using NLOSb instead of default LogNormalShadowing
        double minStdDevNLOSb @unit(dB) = default(4.1 dB);
        double maxStdDevNLOSb @unit(dB) = default(6.8 dB);

        // use same as for NLOSb by default
        double minStdDevNLOSf @unit(dB) = default(4.1 dB);
        double maxStdDevNLOSf @unit(dB) = default(6.8 dB);

        // Following parameters depend on the used map and its traffic demand.
        // Hence, it is hard to give reasonable default values.
        // SmallScaleVariation module reports proper values after each run,
        // i.e. run it once with arbitrary values and set them accordingly in later runs.
        double maxVehicleDensity; // named NV_max in article
        double maxObstacleDensity; // named AS_max in article
}
