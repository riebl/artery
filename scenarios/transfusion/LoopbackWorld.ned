import artery.transfusion.TransfusionLoopback;

network LoopbackWorld extends artery.inet.World
{
    submodules:
        loopback: TransfusionLoopback {}
}
