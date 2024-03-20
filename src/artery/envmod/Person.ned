package artery.envmod;

import artery.envmod.LocalEnvironmentModel;
import artery.inet.Person;

module Person extends artery.inet.Person
{
    parameters:
        @display("i=block/process;is=vs");

    submodules:
        environmentModel: LocalEnvironmentModel {
            @display("p=214,57");
            middlewareModule = default(".middleware");
        }

}
