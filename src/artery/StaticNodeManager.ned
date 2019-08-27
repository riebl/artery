package artery;

simple StaticNodeManager
{
    parameters:
        @display("i=block/network2;is=s");
        @signal[addRoadSideUnit](type=string);

        xml nodes = default(xml("<nodes />"));
        bool directionalAntennas = default(false);
        string rsuType = default("artery.envmod.RSU");
        string rsuPrefix = default("rsu");
        volatile double rsuHeight @unit(m) = default(7.1 m);
}
