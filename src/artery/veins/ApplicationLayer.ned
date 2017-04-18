package artery.veins;

import artery.application.ItsG5Middleware;
import artery.nic.IRadioDriver;
import artery.nic.VeinsRadioDriver;
import org.car2x.veins.base.modules.IBaseApplLayer;

// Middleware as Veins compatiblity application layer
module ApplicationLayer like IBaseApplLayer
{
	parameters:
		int headerLength @unit("bit") = 0; // ignored

	gates:
		input lowerLayerIn;
		output lowerLayerOut;
		input lowerControlIn;
		output lowerControlOut;

	submodules:
		radioDriver : VeinsRadioDriver {
		}

		middleware : ItsG5Middleware {
		}

	connections allowunconnected:
		lowerLayerIn --> radioDriver.lowerLayerIn;
		lowerLayerOut <-- radioDriver.lowerLayerOut;
		lowerControlIn --> radioDriver.lowerControlIn;
		lowerControlOut <-- radioDriver.lowerControlOut;

		middleware.radioDriverIn <-- radioDriver.middleware$o;
		middleware.radioDriverOut --> radioDriver.middleware$i;
}
