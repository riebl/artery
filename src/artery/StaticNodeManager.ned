package artery;

simple StaticNodeManager
{
    parameters:
        xml nodes;
        bool directional = default(false);
        string rsuType = default("artery.envmod.RSU");
        volatile double rsuHeight @unit(m) = default(7.1 m);
}
