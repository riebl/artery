package ots;

module Connector
{
    parameters:
        @display("i=block/network2;is=s");

    submodules:
        core: Core {
        }

        gtuController: <default("BasicGtuLifecycleController")> like GtuLifecycleController {
        }
}
