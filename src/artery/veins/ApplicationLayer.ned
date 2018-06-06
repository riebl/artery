package artery.veins;

import artery.application.IMiddleware;
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

		middleware : <default("VehicleMiddleware")> like IMiddleware {
		}

	connections allowunconnected:
		lowerLayerIn --> radioDriver.lowerLayerIn;
		lowerLayerOut <-- radioDriver.lowerLayerOut;

		middleware.radioDriverIn <-- radioDriver.middleware$o;
		middleware.radioDriverOut --> radioDriver.middleware$i;
}
