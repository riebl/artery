package artery.inet.gemv2;

simple VehicleIndex
{
    parameters:
        @class(gemv2::VehicleIndex);
        string traciModule;
        string visualizerModule;
        double vehicleMargin @unit(m) = default(0.01 m);
}
