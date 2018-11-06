package artery.veins;

import artery.application.IMiddleware;
import artery.networking.Vanetza;
import artery.nic.IRadioDriver;
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
		radioDriver : RadioDriver {
		}

		vanetza: Vanetza {
			*.middlewareModule = absPath("^.middleware");
			*.radioDriverModule = absPath("^.radioDriver");
			runtime.datetime = middleware.datetime;
		}

		middleware : <default("VehicleMiddleware")> like IMiddleware {
		}

	connections allowunconnected:
		lowerLayerIn --> radioDriver.lowerLayerIn;
		lowerLayerOut <-- radioDriver.lowerLayerOut;

		radioDriver.upperLayer <--> vanetza.radioDriverData;
		radioDriver.properties --> vanetza.radioDriverProperties;
}
