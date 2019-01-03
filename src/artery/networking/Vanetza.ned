package artery.networking;

import artery.dcc.IDccEntity;

module Vanetza
{
    parameters:
        *.dccModule = absPath(".dcc");
        *.positionModule = absPath(".position");
        *.routerModule = absPath(".router");
        *.runtimeModule = absPath(".runtime");
        *.securityModule = absPath(".security");

    gates:
        inout radioDriverData;
        input radioDriverProperties;

    submodules:
        position: <default("VehiclePositionProvider")> like IPositionProvider {
            @display("p=150,50;i=misc/globe");
        }

        dcc: <default("FsmDccEntity")> like IDccEntity {
            @display("p=50,150;i=block/buffer");
        }

        security: <default("")> like ISecurityEntity if typename != "" {
            @display("p=250,150;i=device/firewall");
        }

        router: Router {
            @display("p=150,150;i=block/routing");
        }

        runtime: Runtime {
            @display("p=50,50;i=block/timer");
        }

    connections:
        radioDriverData$i --> router.radioDriverData;
        radioDriverData$o <-- dcc.radioDriverData;
        radioDriverProperties --> router.radioDriverProperties;
}

