package artery.envmod;

import artery.envmod.LocalEnvironmentModel;
import artery.inet.RSU;

module RSU extends artery.inet.RSU
{
    submodules:
        environmentModel: LocalEnvironmentModel {
            @display("p=214,57");
            middlewareModule = default(".middleware");
        }
}

