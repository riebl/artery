package artery.envmod;

import artery.inet.PlainPerson;
import artery.envmod.IdentityRegistrant;

module PlainPerson extends artery.inet.PlainPerson
{
    submodules:
        personIdentityRegistrant: IdentityRegistrant {
            parameters:
                @display("p=53,300");
        }

}
