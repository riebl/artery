//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

package artery.nic;

import artery.mac.MacItsG5;
import org.car2x.veins.modules.nic.INic80211p;
import org.car2x.veins.modules.phy.PhyLayer80211p;

//
// This NIC implements an 802.11p network interface card for ETSI ITS-G5
//
module Nic80211pItsG5 like INic80211p
{
	gates:
		input upperLayerIn; // to upper layers
		output upperLayerOut; // from upper layers
		output upperControlOut; // control information
		input upperControlIn; // control information
		input radioIn; // radioIn gate for sendDirect

	submodules:
		phy80211p: PhyLayer80211p {
			@display("p=69,218;i=block/process_s");
		}

		macItsG5: MacItsG5 {
			@display("p=69,82");
		}

	connections:
		radioIn --> phy80211p.radioIn;

		macItsG5.lowerControlOut --> phy80211p.upperControlIn;
		macItsG5.lowerLayerOut --> phy80211p.upperLayerIn;
		phy80211p.upperLayerOut --> macItsG5.lowerLayerIn;
		phy80211p.upperControlOut --> macItsG5.lowerControlIn;

		macItsG5.upperControlIn <-- upperControlIn;
		macItsG5.upperLayerIn <-- upperLayerIn;

		macItsG5.upperLayerOut --> upperLayerOut;
		macItsG5.upperControlOut --> upperControlOut;
}
