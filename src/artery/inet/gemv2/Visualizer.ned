package artery.inet.gemv2;

simple Visualizer
{
    parameters:
        @class(gemv2::Visualizer);
        string backgroundColor = default("Grey");
        string diffractionColor = default("Gold");
        string foliageColorInside = default("ForestGreen");
        string foliageColorOutside = default("LawnGreen");
        string reflectionColorObstacle = default("VioletRed");
        string reflectionColorVehicle = default("Violet");
}
